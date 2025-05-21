// Fill out you copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Slash/DebugMacros.h"
#include "Slash/Public/Components/AttributeComponent.h"
#include "Slash/Public/HUD/HealthBarComponent.h"

#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

#include <type_traits>

// Sets default values
AEnemy::AEnemy(){
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
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Ŀ�곬��һ����Χ, ���������������Ѫ��
	if (CombatTarget) {
		auto TargetLocation = CombatTarget->GetActorLocation();
		auto SelfLocation = GetActorLocation();
		auto DistanceToTarget = (TargetLocation - SelfLocation).Size();
		//ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("DistanceToTarget: %f"), DistanceToTarget));
		if (DistanceToTarget > CombatRadius) {
			CombatTarget = nullptr;
			if (HealthBarWidget) {
				HealthBarWidget->SetVisibility(false);
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	CombatTarget = EventInstigator->GetPawn(); // ����ָ��ת����, ��Ƭ
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}
	// ����������Ϊ Enemy û���κο���, ȫ���˺�ת��Ϊ��ʵ�˺�
	return  DamageAmount;
}

