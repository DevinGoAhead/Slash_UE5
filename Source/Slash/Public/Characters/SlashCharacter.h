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
class AWeapon;

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
	// ��¶����ͼ, ����֪ͨʱ����
	UFUNCTION(BlueprintCallable)
	void Arm();
	// ��¶����ͼ, ����֪ͨʱ����
	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabledType);

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void Attack();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayMontage(FName MontageSction, UAnimMontage* Montage);
private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UCameraComponent* CameraView = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Hair = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Eyebrow = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Item") // ��ʵ���ɼ�
	AItem* OverlappingItem = nullptr;

	AWeapon* EquippedWeapon = nullptr;

	ECharacterStates CharacterState = ECharacterStates::ECS_UnEquiped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionStates ActionState = EActionStates::EAS_Unoccupied;
	
	// ��¶����ͼ, ��������ͼ�޸�, ��������̫��ľ���Ȩ������ͼ
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage = nullptr;
	
	// ��¶����ͼ, ��������ͼ�޸�, ��������̫��ľ���Ȩ������ͼ
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage = nullptr;
};
