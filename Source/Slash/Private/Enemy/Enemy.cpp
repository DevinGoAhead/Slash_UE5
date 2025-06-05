// Fill out you copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Slash/DebugMacros.h"
#include "Slash/Public/Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Characters/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

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

	// 与 Attributes 匹配, 但是这个是要实际显示的
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// 设置 AI Controller 控制角色旋转相关项
	auto MoveCompt = GetCharacterMovement();
	MoveCompt->bOrientRotationToMovement = true;
	MoveCompt->MaxWalkSpeed = WalkSpeed; // 因为 Enemy初始是巡逻状态, 我期望巡逻时是walk 而不是run
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//PawnSensing, 不需要attach 到其它组件
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	PawnSensing->SightRadius = 2000.f; // 实际可调大一些, 这里为了debug 方便
	PawnSensing->SetPeripheralVisionAngle(80.f);
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
	Tags.Add(FName("Enemy"));
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // 初始不可见
		HealthBarWidget->SetHealthPercent(1.f); // 初始化为100%
	}

	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController) {
		EnemyController->ReceiveMoveCompleted.AddDynamic(this, &AEnemy::OnMoveCompleted);
		MoveToNextPatrol();
	}

	if (PawnSensing) {
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSee);
	}

	if (auto World = GetWorld(); World && WeaponClass) {
		EquippedWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		EquippedWeapon->EquipWeapon(GetMesh(), FName("RightHandSocket"), this, this);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	//if (EnemyState == EEnemyStates::EES_Dead) return;
	Super::Tick(DeltaTime);
	UpdateState();
}

void AEnemy::Destroyed() {
	if (EquippedWeapon) {
		EquippedWeapon->Destroy(); // 申请加入 KillPending 队列, 等待适当时机销毁
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

std::optional<double> AEnemy::GetTargetDistance(AActor* Traget) {
	if (!Traget) return std::nullopt;
	return (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
}

void AEnemy::OnPawnSee(APawn* Pawn) {
	// 如果已经处于攻击状态或追逐状态, 那么即使看到玩家, 也不需要改变当前状态
	if (!Pawn->ActorHasTag(FName("EngageableTarget")) 
			|| Pawn->ActorHasTag(FName("Dead"))
			|| CombatTarget) return;

	GetWorldTimerManager().ClearTimer(PatrolTimer); // 如果 Enemy 正在 wait, 则取消 wait, 下一步如何行动由 CheckCombatTarget 决定
	CombatTarget = Cast<AActor>(Pawn);
	UpdateState();
}

void AEnemy::UpdateState() {
	if (EnemyState == EEnemyStates::EES_Dead) return;// 没必要执行任何动作了
	
	auto Opt = GetTargetDistance(CombatTarget);
	//debug 当前状态
	FString CurState = StaticEnum<EEnemyStates>()->GetNameStringByValue(static_cast<int8>(EnemyState));
	ADD_SCREEN_DEBUG(2, FString::Printf(TEXT("EnemyState: %s"), *CurState));

	if (!Opt) {
		if (EnemyState != EEnemyStates::EES_Patrolling) {
			SetState(EEnemyStates::EES_Patrolling);
		}
		return;
	}
	// 以下有战斗目标
	double ToCombatDistance = Opt.value();
	//debug 当前与目标距离
	ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("ToCombatDistance: %.0f"), ToCombatDistance));

	// [0, 攻击范围), 且不处于攻击状态
	if (ToCombatDistance < AttackRadius) {
		if (EnemyState != EEnemyStates::EES_Attacking
			&& EnemyState != EEnemyStates::EES_Engaged) {
			SetState(EEnemyStates::EES_Attacking);
		}
	}
	// [攻击范围, 战斗范围), 且不处于追击状态
	else if (ToCombatDistance < CombatRadius) {
		if (EnemyState != EEnemyStates::EES_Chasing/* || 
			(EnemyState == EEnemyStates::EES_Chasing && GetCharacterMovement()->Velocity.Size2D() < 0.2f)*/) {
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
	else if (ToCombatDistance > PawnSensing->SightRadius) {
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
		//TODO Attacking...
		StartAttackTimer();
		break;
	case EEnemyStates::EES_Chasing:
		//ADD_SCREEN_DEBUG(5, FString("Move For Chasing"));
		EnemyController->StopMovement();
		if(GetDistanceTo(CombatTarget) > AttackRadius) MoveToTarget(CombatTarget);
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
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
		//ADD_SCREEN_DEBUG(7, FString("Lose Interest"));
		EnemyController->StopMovement();
		MoveToNextPatrol();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	default:
		break;
	}
}

void AEnemy::PatrolTimerFinished() {
	MoveToNextPatrol();
}

void AEnemy::MoveToNextPatrol() {
	//ADD_SCREEN_DEBUG(5, FString("Move For Patrolling"));
	auto Num = PatrolTargets.Num();
	// 如果是静态 巡逻点, 那么直接在 BeginPlay 中检查一次也是可以的
	// 我了解到似乎有些游戏存在游戏中动态改变巡逻点的机制, 故保留在 StartNextMove 中
	if (Num == 0) {
		//ADD_SCREEN_DEBUG(5, FString("PatrolTargets.Num == 0"));
		return;
	}

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
	MoveRequst.SetGoalActor(Target); // 
	// 与目标点距离 < 5 即判定完成 Move, 停止移动, 实际存在误差, 可能100就判定完成任务了
	// 这里希望尽可能离目标近, 然后在需要执行的动作中进一步判断距离补足精度
	MoveRequst.SetAcceptanceRadius(100);
	EnemyController->MoveTo(MoveRequst);
}

void AEnemy::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) {
	if (Result == EPathFollowingResult::Success && !CombatTarget) {
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(MinPatrolWait, MaxPatrolWait));
	}
}

void AEnemy::StartAttackTimer() {
	EnemyState = EEnemyStates::EES_Engaged;
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::AttackTimerFinished, FMath::RandRange(MinAttackWait, MaxAttackWait));
}

void AEnemy::AttackTimerFinished() {
	//ADD_SCREEN_DEBUG(8, FString("AttackTimerFinished"));
	Attack(); 
}

void AEnemy::Attack() {
	Super::Attack();
	if (EnemyState == EEnemyStates::EES_Dead) return;
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) {
		CombatTarget = nullptr;
		return;
	}
	//if (auto Opt = GetTargetDistance(CombatTarget); Opt && AttackMontage) {
		//if (Opt.value() < 130.) {
			EnemyController->StopMovement(); // 停止 move
			//// 转向Combat Target 再攻击
			//FRotator RotationToTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
			////SetActorRotation(RotationToTarget);
			//EnemyController->SetControlRotation(RotationToTarget); // 更顺滑
			int32 index = FMath::RandRange(1, AttackMontage->CompositeSections.Num());
			PlayMontage(FName("Attack_" + FString::FromInt(index)), AttackMontage);
	//	}
	//	else {
	//		//ADD_SCREEN_DEBUG(5, FString("Move For Attacking"));
	//		EnemyController->StopMovement(); // 停止 move
	//		MoveToTarget(CombatTarget);
	//		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	//		// 如果不执行if 将不会最终执行AttackEnd, 则不会将 EnemyState 设置为 EES_None, 导致无法在Attack与 Engaged 之间转换
	//		ResetAttackState();
	//	}
	//}
}

//void AEnemy::ResetAttackState() {
//	AttackEnd();
//}

// 在 Montage Animation 结束时的 Notify 中调用
void AEnemy::AttackEnd() {
	EnemyState = EEnemyStates::EES_None;
	//UE_LOG(LogTemp, Warning, TEXT("AEnemy::AttackEnd() "));
	UpdateState();
}

FVector AEnemy::GetRotationWarpTarget() {
	if(!CombatTarget) return FVector();
	return CombatTarget->GetActorLocation();
}

FVector AEnemy::GetTranslationWarpTarget() {
	if (!CombatTarget) return FVector();
	auto TargetLocation = CombatTarget->GetActorLocation();
	auto TragetToMe = (GetActorLocation() - TargetLocation).GetSafeNormal();
	return TargetLocation + TragetToMe * WarpTargetDistance;
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!CombatTarget) { // 按照逻辑, 这里放在 GetHited_Implementation 比较合适, 但是我不想再增加一个成员变量存储 EventInstigator 了
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		CombatTarget = EventInstigator->GetPawn(); // 子类指针转父类, 切片
		UpdateState();
	}

	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
		//if (Attributes->IsAlive() && EnemyState != EEnemyStates::EES_None) {
		//	// 这里主要针对正在Enemy 正在攻击时, 受到玩家攻击被打断, 导致无法执行完成攻击动画,无法重置状态为None
		//	// Attributes->IsAlive() 主要是由于, 如果是最后一击, 就没必要再重新攻击了
		//	//ADD_SCREEN_DEBUG(8, FString("Reset Attack While Hited"));
		//	ResetAttackState();
		//}
	}
	
	// 这里暂先认为 Enemy 没有任何抗性, 全部伤害转换为真实伤害
	return  DamageAmount;
}

void AEnemy::GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) {
	Super::GetHited_Implementation(Impactpoint, Hitter);

	if (HealthBarWidget && EnemyState != EEnemyStates::EES_Dead) {
		HealthBarWidget->SetVisibility(true);
	}

	if (EnemyState != EEnemyStates::EES_Dead && EnemyState != EEnemyStates::EES_None) {
		// 这里主要针对正在Enemy 正在攻击时, 受到玩家攻击被打断, 导致无法执行完成攻击动画,无法重置状态为None
		// Attributes->IsAlive()(EnemyState != EEnemyStates::EES_Dead ) 主要是由于, 如果是最后一击, 就没必要再重新攻击了
		//ADD_SCREEN_DEBUG(8, FString("Reset Attack While Hited"));
		//ResetAttackState(); // 会失败, 导致状态不重置, 原因不知
		GetWorldTimerManager().ClearTimer(AttackTimer);
		EnemyState = EEnemyStates::EES_None;
		UpdateState();
	}
}

void AEnemy::Die() {
	Super::Die();

	EnemyState = EEnemyStates::EES_Dead;
	// Tick会在 GetTargetDistance 中调用 GetActorLocation()
	// SetLifeSpan 之后 Enemy 已经销毁, 会导致崩溃
	CombatTarget = nullptr; // 否则会在GetTargetDistance

	// 后续处理
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
	GetWorld()->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
	SetLifeSpan(5.f); // 3s 后对象即销毁
}








