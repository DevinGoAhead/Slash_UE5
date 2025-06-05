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

	// �� Attributes ƥ��, ���������Ҫʵ����ʾ��
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// ���� AI Controller ���ƽ�ɫ��ת�����
	auto MoveCompt = GetCharacterMovement();
	MoveCompt->bOrientRotationToMovement = true;
	MoveCompt->MaxWalkSpeed = WalkSpeed; // ��Ϊ Enemy��ʼ��Ѳ��״̬, ������Ѳ��ʱ��walk ������run
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//PawnSensing, ����Ҫattach ���������
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	PawnSensing->SightRadius = 2000.f; // ʵ�ʿɵ���һЩ, ����Ϊ��debug ����
	PawnSensing->SetPeripheralVisionAngle(80.f);
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
	Tags.Add(FName("Enemy"));
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // ��ʼ���ɼ�
		HealthBarWidget->SetHealthPercent(1.f); // ��ʼ��Ϊ100%
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
		EquippedWeapon->Destroy(); // ������� KillPending ����, �ȴ��ʵ�ʱ������
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
	// ����Ѿ����ڹ���״̬��׷��״̬, ��ô��ʹ�������, Ҳ����Ҫ�ı䵱ǰ״̬
	if (!Pawn->ActorHasTag(FName("EngageableTarget")) 
			|| Pawn->ActorHasTag(FName("Dead"))
			|| CombatTarget) return;

	GetWorldTimerManager().ClearTimer(PatrolTimer); // ��� Enemy ���� wait, ��ȡ�� wait, ��һ������ж��� CheckCombatTarget ����
	CombatTarget = Cast<AActor>(Pawn);
	UpdateState();
}

void AEnemy::UpdateState() {
	if (EnemyState == EEnemyStates::EES_Dead) return;// û��Ҫִ���κζ�����
	
	auto Opt = GetTargetDistance(CombatTarget);
	//debug ��ǰ״̬
	FString CurState = StaticEnum<EEnemyStates>()->GetNameStringByValue(static_cast<int8>(EnemyState));
	ADD_SCREEN_DEBUG(2, FString::Printf(TEXT("EnemyState: %s"), *CurState));

	if (!Opt) {
		if (EnemyState != EEnemyStates::EES_Patrolling) {
			SetState(EEnemyStates::EES_Patrolling);
		}
		return;
	}
	// ������ս��Ŀ��
	double ToCombatDistance = Opt.value();
	//debug ��ǰ��Ŀ�����
	ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("ToCombatDistance: %.0f"), ToCombatDistance));

	// [0, ������Χ), �Ҳ����ڹ���״̬
	if (ToCombatDistance < AttackRadius) {
		if (EnemyState != EEnemyStates::EES_Attacking
			&& EnemyState != EEnemyStates::EES_Engaged) {
			SetState(EEnemyStates::EES_Attacking);
		}
	}
	// [������Χ, ս����Χ), �Ҳ�����׷��״̬
	else if (ToCombatDistance < CombatRadius) {
		if (EnemyState != EEnemyStates::EES_Chasing/* || 
			(EnemyState == EEnemyStates::EES_Chasing && GetCharacterMovement()->Velocity.Size2D() < 0.2f)*/) {
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
		EnemyController->StopMovement(); // ֹͣ move
		// ����Ѫ��
		if (HealthBarWidget) {
			HealthBarWidget->SetVisibility(false);
		}
		break;
	case EEnemyStates::EES_Patrolling:
		CombatTarget = nullptr; // ��ȫʧȥ��Ȥ(ս��Ŀ��)
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
	// ����Ǿ�̬ Ѳ�ߵ�, ��ôֱ���� BeginPlay �м��һ��Ҳ�ǿ��Ե�
	// ���˽⵽�ƺ���Щ��Ϸ������Ϸ�ж�̬�ı�Ѳ�ߵ�Ļ���, �ʱ����� StartNextMove ��
	if (Num == 0) {
		//ADD_SCREEN_DEBUG(5, FString("PatrolTargets.Num == 0"));
		return;
	}

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
	MoveRequst.SetGoalActor(Target); // 
	// ��Ŀ������ < 5 ���ж���� Move, ֹͣ�ƶ�, ʵ�ʴ������, ����100���ж����������
	// ����ϣ����������Ŀ���, Ȼ������Ҫִ�еĶ����н�һ���жϾ��벹�㾫��
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
			EnemyController->StopMovement(); // ֹͣ move
			//// ת��Combat Target �ٹ���
			//FRotator RotationToTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
			////SetActorRotation(RotationToTarget);
			//EnemyController->SetControlRotation(RotationToTarget); // ��˳��
			int32 index = FMath::RandRange(1, AttackMontage->CompositeSections.Num());
			PlayMontage(FName("Attack_" + FString::FromInt(index)), AttackMontage);
	//	}
	//	else {
	//		//ADD_SCREEN_DEBUG(5, FString("Move For Attacking"));
	//		EnemyController->StopMovement(); // ֹͣ move
	//		MoveToTarget(CombatTarget);
	//		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	//		// �����ִ��if ����������ִ��AttackEnd, �򲻻Ὣ EnemyState ����Ϊ EES_None, �����޷���Attack�� Engaged ֮��ת��
	//		ResetAttackState();
	//	}
	//}
}

//void AEnemy::ResetAttackState() {
//	AttackEnd();
//}

// �� Montage Animation ����ʱ�� Notify �е���
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
	if (!CombatTarget) { // �����߼�, ������� GetHited_Implementation �ȽϺ���, �����Ҳ���������һ����Ա�����洢 EventInstigator ��
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		CombatTarget = EventInstigator->GetPawn(); // ����ָ��ת����, ��Ƭ
		UpdateState();
	}

	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
		//if (Attributes->IsAlive() && EnemyState != EEnemyStates::EES_None) {
		//	// ������Ҫ�������Enemy ���ڹ���ʱ, �ܵ���ҹ��������, �����޷�ִ����ɹ�������,�޷�����״̬ΪNone
		//	// Attributes->IsAlive() ��Ҫ������, ��������һ��, ��û��Ҫ�����¹�����
		//	//ADD_SCREEN_DEBUG(8, FString("Reset Attack While Hited"));
		//	ResetAttackState();
		//}
	}
	
	// ����������Ϊ Enemy û���κο���, ȫ���˺�ת��Ϊ��ʵ�˺�
	return  DamageAmount;
}

void AEnemy::GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) {
	Super::GetHited_Implementation(Impactpoint, Hitter);

	if (HealthBarWidget && EnemyState != EEnemyStates::EES_Dead) {
		HealthBarWidget->SetVisibility(true);
	}

	if (EnemyState != EEnemyStates::EES_Dead && EnemyState != EEnemyStates::EES_None) {
		// ������Ҫ�������Enemy ���ڹ���ʱ, �ܵ���ҹ��������, �����޷�ִ����ɹ�������,�޷�����״̬ΪNone
		// Attributes->IsAlive()(EnemyState != EEnemyStates::EES_Dead ) ��Ҫ������, ��������һ��, ��û��Ҫ�����¹�����
		//ADD_SCREEN_DEBUG(8, FString("Reset Attack While Hited"));
		//ResetAttackState(); // ��ʧ��, ����״̬������, ԭ��֪
		GetWorldTimerManager().ClearTimer(AttackTimer);
		EnemyState = EEnemyStates::EES_None;
		UpdateState();
	}
}

void AEnemy::Die() {
	Super::Die();

	EnemyState = EEnemyStates::EES_Dead;
	// Tick���� GetTargetDistance �е��� GetActorLocation()
	// SetLifeSpan ֮�� Enemy �Ѿ�����, �ᵼ�±���
	CombatTarget = nullptr; // �������GetTargetDistance

	// ��������
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
	GetWorld()->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
	SetLifeSpan(5.f); // 3s ���������
}








