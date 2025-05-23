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
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ������Ϸ�е�Weapon ����Ϊ���� Pawn, ������ﲻ������ Pawn
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // �������
	SKMesh->SetGenerateOverlapEvents(true); //��������overlap�¼�
	
	// ���������ײ��Ӧ
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	// ����Ҫ���ӵ����, ��Ϊû��mesh, Ҳû��λ�û���������, �����洢����, ����Ҫ��ʾ
	Attributes = CreateDefaultSubobject<UAttributeComponent>(FName("Attributes"));

	// �� Attributes ƥ��, ���������Ҫʵ����ʾ��
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// ���� AI Controller ���ƽ�ɫ��ת�����
	auto MoveCompt = GetCharacterMovement();
	MoveCompt->bOrientRotationToMovement = true;
	MoveCompt->MaxWalkSpeed = 130; // ��Ϊ Enemy��ʼ��Ѳ��״̬, ������Ѳ��ʱ��walk ������run
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//PawnSensing, ����Ҫattach ���������
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	PawnSensing->SightRadius = 2000.f; // ʵ�ʿɵ���һЩ, ����Ϊ��debug ����
	PawnSensing->SetPeripheralVisionAngle(80.f);
}

void AEnemy::Die() {
	// ��������
	int32 index = FMath::RandRange(int32(1), int32(DeathMontage->CompositeSections.Num()));
	//ADD_SCREEN_DEBUG(2, FString::FromInt(index));
	DeathPose = static_cast<EDeathPose>(index); // UE Cast ת��ʧ��, ��ʹ�� c++ �⺯��
	PlayMontage(FName("Death_" + FString::FromInt(index)), DeathMontage);

	// ��������
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // ��ֹ������ײ
	SetLifeSpan(3.f); // 5s ���������
}

void AEnemy::MoveToNextPatrol() {
	auto Num = PatrolTargets.Num();
	// ����Ǿ�̬ Ѳ�ߵ�, ��ôֱ���� BeginPlay �м��һ��Ҳ�ǿ��Ե�
	// ���˽⵽�ƺ���Щ��Ϸ������Ϸ�ж�̬�ı�Ѳ�ߵ�Ļ���, �ʱ����� StartNextMove ��
	if (Num == 0) return;

	// ȷ����һ�ε�Ѳ�ߵ� != ��ǰѲ�ߵ�
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
	MoveRequst.SetGoalActor(Target); // Ѳ��Ŀ��
	MoveRequst.SetAcceptanceRadius(50); // ��Ŀ������ < 50 ���ж����Move, ֹͣ�ƶ�, ʵ�ʴ������, ����100���ж����������
	EnemyController->MoveTo(MoveRequst);
}

void AEnemy::UpdateState() {
	auto Opt = GetTargetDistance(CombatTarget);
	FString CurState = StaticEnum<EEnemyStates>()->GetNameStringByValue(static_cast<int8>(EnemyState));
	ADD_SCREEN_DEBUG(2, FString::Printf(TEXT("EnemyState: %s"), *CurState));
	if (!Opt) return; // ��ս��Ŀ��
	// ������ս��Ŀ��
	double ToCombatDistance = Opt.value();
	ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("ToCombatDistance: %.0f"), ToCombatDistance));

	// [0, ������Χ), �Ҳ����ڹ���״̬
	if (ToCombatDistance < AttackRadius ) {
		if(EnemyState != EEnemyStates::EES_Attacking) {
			SetState(EEnemyStates::EES_Attacking);
		}
	}
	// [������Χ, ս����Χ), �Ҳ�����׷��״̬
	else if (ToCombatDistance < CombatRadius) {
		if (EnemyState != EEnemyStates::EES_Chasing) {
			SetState(EEnemyStates::EES_Chasing);
		}
	}
	// [ս����Χ, ���߷�Χ) , �Ҳ����ھ���״̬״̬
	else if (ToCombatDistance < PawnSensing->SightRadius) {
		if (EnemyState != EEnemyStates::EES_Alerting) {
			SetState(EEnemyStates::EES_Alerting);
		}
	}
	// [���߷�Χ, ��), �Ҳ�����Ѳ��״̬
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
		EnemyController->StopMovement(); // ֹͣ move
		//TODO Attacking...
		break;
	case EEnemyStates::EES_Chasing:
		MoveToTarget(CombatTarget);
		GetCharacterMovement()->MaxWalkSpeed = 300;
		break;
	case EEnemyStates::EES_Alerting:
		EnemyController->StopMovement(); // ֹͣ move
		// ����Ѫ��
		if (HealthBarWidget) {
			HealthBarWidget->SetVisibility(false);
		}
		break;
	case EEnemyStates::EES_Patrolling:
		CombatTarget = nullptr; // ��ȫʧȥ��Ȥ(ս��Ŀ��)
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
	FVector ToHit = (Impactpoint - GetActorLocation()).GetSafeNormal(); // ��һ��
	auto Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit))); // �������Ƕ�

	auto CrossValue = FVector::CrossProduct(Forward, ToHit); // ���ȷ����Χ
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
		AnimInstance->Montage_Play(Montage);// ׼������״̬
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // ��ʼ���ɼ�
		HealthBarWidget->SetHealthPercent(1.f); // ��ʼ��Ϊ100%
	}
	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController) {
		EnemyController->ReceiveMoveCompleted.AddDynamic(this, &AEnemy::OnMoveCompleted);
		MoveToNextPatrol();
		// ����Ϊ debug ��Ҫ
		//FAIMoveRequest MoveRequst;
		//MoveRequst.SetGoalActor(PatrolTarget); // Ѳ��Ŀ��
		//MoveRequst.SetAcceptanceRadius(100); // ��Ŀ������ < 100 ��ֹͣ�ƶ�

		//FNavPathSharedPtr NavPath;
		//EnemyController->MoveTo(MoveRequst, &NavPath);
		//if (NavPath->IsValid()) { // ����ֱ�Ӽ�� PathPoints ��Num �Ƿ� > 1
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
	// ����Ѿ����ڹ���״̬��׷��״̬, ��ô��ʹ�������, Ҳ����Ҫ�ı䵱ǰ״̬
	if (!Pawn->ActorHasTag(FName("SlashCharacter"))  || CombatTarget) return;
	
	GetWorldTimerManager().ClearTimer(PatrolTimer); // ��� Enemy ���� wait, ��ȡ�� wait, ��һ������ж��� CheckCombatTarget ����
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
		CombatTarget = EventInstigator->GetPawn(); // ����ָ��ת����, ��Ƭ
		UpdateState();
	}
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}
	// ����������Ϊ Enemy û���κο���, ȫ���˺�ת��Ϊ��ʵ�˺�
	return  DamageAmount;
}

