// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacerTypes.h"
#include "SlashAnimInstance.generated.h"

class ASlashCharacter;
class UCharacterMovementComponent;

UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* SlashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling; // �Ƿ���������

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	ECharacterStates CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	EActionStates ActonState;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	EDeathPose DeathPose;
};
