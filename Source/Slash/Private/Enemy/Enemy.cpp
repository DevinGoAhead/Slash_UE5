// Fill out you copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Slash/DebugMacros.h"
#include "Slash/Public/Components/AttributeComponent.h"
#include "Slash/Public/HUD/HealthBarComponent.h"
#include "Slash/Public/Characters/SlashCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"
#include "AIController.h"
#include "AITypes.h"
#include "NavigationSystemTypes.h"
#include "NavigationData.h"
#include "AI/Navigation/NavigationTypes.h"

#include <type_traits>

// Sets default values
AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	auto SKMesh = GetMesh();
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 由于游戏中的Weapon 设置为忽略 Pawn, 因而这里不能设置 Pawn
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 忽略相机
	SKMesh->SetGenerateOverlapEvents(true); //启用生成overlap事件
	
	// 胶囊组件碰撞响应
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	// 不需要附加到组件, 因为没有mesh, 也没有位置或其它属性, 仅仅存储数据, 不需要显示
	Attributes = CreateDefaultSubobject<UAttributeComponent>(FName("Attributes"));

	// 与 Attributes 匹配, 但是这个是要实际显示的
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// 设置 AI Controller 控制角色旋转相关项
	auto MoveCompt = GetCharacterMovement();
	MoveCompt->bOrientRotationToMovement = true;
	MoveCompt->MaxWalkSpeed = 130; // 因为 Enemy初始是巡逻状态, 我期望巡逻时是walk 而不是run
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//PawnSensing, 不需要attach 到其它组件
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	PawnSensing->SightRadius = 2000.f; // 实际可调大一些, 这里为了debug 方便
	PawnSensing->SetPeripheralVisionAngle(80.f);
}

void AEnemy::Die() {
	// 死亡动画
	int32 index = FMath::RandRange(int32(1), int32(DeathMontage->CompositeSections.Num()));
	//ADD_SCREEN_DEBUG(2, FString::FromInt(index));
	DeathPose = static_cast<EDeathPose>(index); // UE Cast 转换失败, 故使用 c++ 库函数
	PlayMontage(FName("Death_" + FString::FromInt(index)), DeathMontage);

	// 后续处理
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 禁止胶囊碰撞
	SetLifeSpan(3.f); // 5s 后对象即销毁
}

void AEnemy::MoveToNextPatrol() {
	auto Num = PatrolTargets.Num();
	// 如果是静态 巡逻点, 那么直接在 BeginPlay 中检查一次也是可以的
	// 我了解到似乎有些游戏存在游戏中动态改变巡逻点的机制, 故保留在 StartNextMove 中
	if (Num == 0) return;

	// 确保下一次的巡逻点 != 当前巡逻点
	int8 Index;
	do {
		Index = FMath::RandRange(0, Num - 1);
	} while (Index == PatrolTargetIndex);
	
	PatrolTargetIndex = Index;
	PatrolTarget = PatrolTargets[PatrolTargetIndex];
	MoveToTarget(PatrolTarget);
}

void AEnemy::MoveToTarget(AActor* Target) {
	FAIMoveRequest MoveRequst;
	MoveRequst.SetGoalActor(Target); // 巡逻目标
	MoveRequst.SetAcceptanceRadius(50); // 与目标点距离 < 50 即判定完成Move, 停止移动, 实际存在误差, 可能100就判定完成任务了
	EnemyController->MoveTo(MoveRequst);
}

void AEnemy::UpdateState() {
	auto Opt = GetTargetDistance(CombatTarget);
	FString CurState = StaticEnum<EEnemyStates>()->GetNameStringByValue(static_cast<int8>(EnemyState));
	ADD_SCREEN_DEBUG(2, FString::Printf(TEXT("EnemyState: %s"), *CurState));
	if (!Opt) return; // 无战斗目标
	// 以下有战斗目标
	double ToCombatDistance = Opt.value();
	ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("ToCombatDistance: %.0f"), ToCombatDistance));

	// [0, 攻击范围), 且不处于攻击状态
	if (ToCombatDistance < AttackRadius ) {
		if(EnemyState != EEnemyStates::EES_Attacking) {
			SetState(EEnemyStates::EES_Attacking);
		}
	}
	// [攻击范围, 战斗范围), 且不处于追击状态
	else if (ToCombatDistance < CombatRadius) {
		if (EnemyState != EEnemyStates::EES_Chasing) {
			SetState(EEnemyStates::EES_Chasing);
		}
	}
	// [战斗范围, 视线范围) , 且不处于警觉状态状态
	else if (ToCombatDistance < PawnSensing->SightRadius) {
		if (EnemyState != EEnemyStates::EES_Alerting) {
			SetState(EEnemyStates::EES_Alerting);
		}
	}
	// [视线范围, ∞), 且不处于巡逻状态
	else if(ToCombatDistance > PawnSensing->SightRadius){
		if (EnemyState != EEnemyStates::EES_Patrolling) {
			SetState(EEnemyStates::EES_Patrolling);
		}
	}
}

void AEnemy::SetState(EEnemyStates NewState) {
	if (NewState == EnemyState) return;
	EnemyState = NewState;
	switch (EnemyState) {
	case EEnemyStates::EES_Attacking:
		EnemyController->StopMovement(); // 停止 move
		//TODO Attacking...
		break;
	case EEnemyStates::EES_Chasing:
		MoveToTarget(CombatTarget);
		GetCharacterMovement()->MaxWalkSpeed = 300;
		break;
	case EEnemyStates::EES_Alerting:
		EnemyController->StopMovement(); // 停止 move
		// 隐藏血条
		if (HealthBarWidget) {
			HealthBarWidget->SetVisibility(false);
		}
		break;
	case EEnemyStates::EES_Patrolling:
		CombatTarget = nullptr; // 完全失去兴趣(战斗目标)
		MoveToNextPatrol();
		GetCharacterMovement()->MaxWalkSpeed = 130;
		break;
	default:
		break;
	}
}

void AEnemy::PatrolTimerFinished() {
	MoveToNextPatrol();
}

std::optional<double> AEnemy::GetTargetDistance(AActor* Traget) {
	if (!Traget) return std::nullopt;
	return (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
}

void AEnemy::GetHited_Implementation(const FVector& Impactpoint) {
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
	if (Attributes) {
		if (Attributes->IsAlive() && HitedMontage) {
			DeathPose = EDeathPose::EDP_Alive;
			PlayMontage(FName("HitedReact_" + GetHitedDirection(Impactpoint)), HitedMontage);
		}
		else if (!Attributes->IsAlive() && DeathMontage){
			Die();
		}
	}
	if (HitedSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitedSound, Impactpoint);
	}
	if (HitedParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitedParticle, Impactpoint);
	}
}

FString AEnemy::GetHitedDirection(const FVector& Impactpoint) {
	FVector Forward = GetActorForwardVector();
	FVector ToHit = (Impactpoint - GetActorLocation()).GetSafeNormal(); // 归一化
	auto Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit))); // 点积反算角度

	auto CrossValue = FVector::CrossProduct(Forward, ToHit); // 叉积确定范围
	if (CrossValue.Z < 0.) {
		Theta *= -1.;
	}
	if (Theta >= -135. && Theta < -45.) { return "Left"; }
	else if (Theta >= -45. && Theta < 45.) { return "Front"; }
	else if (Theta >= 45. && Theta < 135.) { return "Right"; }
	else { return "Back"; }
}

void AEnemy::PlayMontage(FName MontageSction, UAnimMontage* Montage) {
	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(Montage);// 准备播放状态
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // 初始不可见
		HealthBarWidget->SetHealthPercent(1.f); // 初始化为100%
	}
	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController) {
		EnemyController->ReceiveMoveCompleted.AddDynamic(this, &AEnemy::OnMoveCompleted);
		MoveToNextPatrol();
		// 以下为 debug 需要
		//FAIMoveRequest MoveRequst;
		//MoveRequst.SetGoalActor(PatrolTarget); // 巡逻目标
		//MoveRequst.SetAcceptanceRadius(100); // 与目标点距离 < 100 即停止移动

		//FNavPathSharedPtr NavPath;
		//EnemyController->MoveTo(MoveRequst, &NavPath);
		//if (NavPath->IsValid()) { // 或者直接检查 PathPoints 的Num 是否 > 1
		//	ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("%d"), NavPath->GetPathPoints().Num()));
		//	for (const auto& Point : NavPath->GetPathPoints()) {
		//		ADD_SCREEN_DEBUG(4, FString::Printf(TEXT("%d"), Point.Location.Z));
		//		DRAW_DEBUG_SPHERE(this, Point.Location);
		//	}
		//}
	}
	if (PawnSensing) {
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSee);
	}
}

void AEnemy::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) {
	if (Result == EPathFollowingResult::Success) {
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(MinWaitTime, MaxWaitTime));
	}
}

void AEnemy::OnPawnSee(APawn* Pawn) {
	// 如果已经处于攻击状态或追逐状态, 那么即使看到玩家, 也不需要改变当前状态
	if (!Pawn->ActorHasTag(FName("SlashCharacter"))  || CombatTarget) return;
	
	GetWorldTimerManager().ClearTimer(PatrolTimer); // 如果 Enemy 正在 wait, 则取消 wait, 下一步如何行动由 CheckCombatTarget 决定
	CombatTarget = Cast<AActor>(Pawn);
	UpdateState();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateState();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (!CombatTarget) {
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		CombatTarget = EventInstigator->GetPawn(); // 子类指针转父类, 切片
		UpdateState();
	}
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}
	// 这里暂先认为 Enemy 没有任何抗性, 全部伤害转换为真实伤害
	return  DamageAmount;
}

