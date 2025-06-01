// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AWeapon;
class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UAttributeComponent;

#include "CharacerTypes.h"
#include "Slash/Public/Interfaces/HitInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;
	virtual void Attack();
	FString GetHitedDirection(const FVector& HitterLocation);
	virtual void Die();
	void PlayMontage(const FName& MontageSction, UAnimMontage* Montage);
	FName RandomMontageSection(const FString& Prefix, UAnimMontage* Montage);
	void ResetAttackState();
	// 暴露给蓝图, 动画通知时调用
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabledType);

protected:
	AWeapon* EquippedWeapon = nullptr;

	// 暴露给蓝图, 且允许蓝图修改, 将播放蒙太奇的决定权留给蓝图
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* HitedMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase* HitedSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitedParticle = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UAttributeComponent* Attributes = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose = EDeathPose::EDP_NoState;
};
