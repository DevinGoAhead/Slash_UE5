// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Slash/DebugMacros.h"
#include "Slash/Public/Items/Weapons/Weapon.h"
#include "Slash/Public/Components/AttributeComponent.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	// ����Ҫ���ӵ����, ��Ϊû��mesh, Ҳû��λ�û���������, �����洢����, ����Ҫ��ʾ
	Attributes = CreateDefaultSubobject<UAttributeComponent>(FName("Attributes"));

	auto SKMesh = GetMesh();
	SKMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SKMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap); // �� Weapon.CollisionBox ����
	SKMesh->SetGenerateOverlapEvents(true); //��������overlap�¼�

	auto Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Block);

	Capsule->SetGenerateOverlapEvents(true); //��������overlap�¼�
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// �����ɫ�ֳ�����������, ���Է�������������Ҳ���������Ч��, �� Montage Notify Event ����
void ABaseCharacter::SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabledType) {
	if (EquippedWeapon) {
		UBoxComponent* Box = EquippedWeapon->GetCollisionBox();
		if (Box) {
			Box->SetCollisionEnabled(CollisionEnabledType);
			EquippedWeapon->SetIgnoreActorsEmpty();
		}
	}
}

void ABaseCharacter::GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) {
	if (EquippedWeapon) {
		SetWeaponBoxCollision(ECollisionEnabled::NoCollision);
	}
	if (Attributes) {
		if (Attributes->IsAlive() && HitedMontage) {
			//DeathPose = EDeathPose::EDP_Alive;
			PlayMontage(FName("HitedReact_" + GetHitedDirection(Hitter->GetActorLocation())), HitedMontage);
		}
		else if (!Attributes->IsAlive() && DeathMontage) {
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

FString ABaseCharacter::GetHitedDirection(const FVector& HitterLocation) {
	FVector Forward = GetActorForwardVector();
	FVector ToHit = (HitterLocation - GetActorLocation()).GetSafeNormal(); // ��һ��
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

void ABaseCharacter::Die() {
	if (!DeathMontage) return;
	// ��������
	int32 index = FMath::RandRange(1, DeathMontage->CompositeSections.Num());
	ADD_SCREEN_DEBUG(6, FString::FromInt(index));
	EDeathPose Pose = static_cast<EDeathPose>(index); // UE Cast ת��ʧ��, ��ʹ�� c++ �⺯��
	//TEnumAsByte<EDeathPose> Pose(index);

	if (Pose < EDeathPose::EDP_MAX) {
		DeathPose = Pose;
		PlayMontage(FName("Death_" + FString::FromInt(index)), DeathMontage);
	}
	else {
		ADD_SCREEN_DEBUG(5, "!!!Pose Bounds!!!");
	}
}

void ABaseCharacter::Attack() {
	if (AttackMontage) return;
}

void ABaseCharacter::AttackEnd() {}

void ABaseCharacter::ResetAttackState() {
	AttackEnd();
}
// Montage ����Ϊ��, ������ܱ���
void ABaseCharacter::PlayMontage(const FName& MontageSction, UAnimMontage* Montage) {
	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(Montage);// ׼������״̬
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

// Montage ����Ϊ��, ������ܱ���
FName ABaseCharacter::RandomMontageSection(const FString& Prefix, UAnimMontage* Montage) {
	int32 index = FMath::RandRange(1, Montage->CompositeSections.Num());
	//ADD_SCREEN_DEBUG(4, FString::FromInt(index));
	return FName(Prefix + FString::FromInt(index));
}
