// ill out your copyright notice in the Description page of Project Settings.

#pragma once

class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UAttributeComponent;
class UWidgetComponent;
class UHealthBarComponent;
class AController;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "Slash/Public/Characters/CharacerTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void GetHited_Implementation(const FVector& Impactpoint) override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
							AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
private:
	void PlayMontage(FName MontageSction, UAnimMontage* Montage);
	FString GetHitedDirection(const FVector& Impactpoint);
	void Die();

private:
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

	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UHealthBarComponent* HealthBarWidget = nullptr; //Component, 可以绑定widget 的 component

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 3500.f; // 经测试, 默认单位是厘米

	UPROPERTY(EditAnywhere, Category = "Combat")
	AActor* CombatTarget = nullptr;
};
