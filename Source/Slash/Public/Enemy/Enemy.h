// ill out your copyright notice in the Description page of Project Settings.

#pragma once

class UAnimMontage;
class USoundBase;
class UParticleSystem;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void GetHited(const FVector& Impactpoint) override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
private:
	void PlayMontage(FName MontageSction, UAnimMontage* Montage);
	FString GetHitedDirection(const FVector& Impactpoint);

private:
	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* HitedMontage;

	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase* HitedSound;

	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitedParticle;
};
