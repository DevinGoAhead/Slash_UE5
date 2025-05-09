// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacerTypes.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterStates GetCharacterState() { return CharacterState; }

protected:
	virtual void BeginPlay() override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Equip();
	void Attack();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UCameraComponent* CameraView;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Eyebrow;

	UPROPERTY(VisibleInstanceOnly, Category = "Item") // ��ʵ���ɼ�
	AItem* OverlappingItem;

	ECharacterStates CharacterState;
	
	// ��¶����ͼ, ��������ͼ�޸�, ��������̫��ľ���Ȩ������ͼ
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionStates ActionState;
};
