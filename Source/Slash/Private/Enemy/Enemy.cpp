// Fill out you copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	auto SKMesh = GetMesh();
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 由于游戏中的Weapon 设置为忽略 Pawn, 因而这里不能设置 Pawn
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 忽略相机
	SKMesh->SetGenerateOverlapEvents(true); //启用生成overlap事件
	
	// 胶囊
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AEnemy::GetHited(const FVector& Impactpoint) {
	if (HitedMontage) {
		PlayMontage(FName("HitedReact_" + GetHitedDirection(Impactpoint)), HitedMontage);
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
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

