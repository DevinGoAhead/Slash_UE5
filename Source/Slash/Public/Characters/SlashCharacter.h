// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;

#include "Characters/BaseCharacter.h"
#include "SlashCharacter.generated.h"

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) override;
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterStates GetCharacterState() { return CharacterState; }

protected:
	virtual void BeginPlay() override;
	virtual void Attack() override;
	virtual void Die() override;

	virtual void AttackEnd() override;
	// 暴露给蓝图, 动画通知时调用
	UFUNCTION(BlueprintCallable)
	void Arm();
	// 暴露给蓝图, 动画通知时调用
	UFUNCTION(BlueprintCallable)
	void Disarm();

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UCameraComponent* CameraView = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Hair = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UGroomComponent* Eyebrow = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Item") // 仅实例可见
	AItem* OverlappingItem = nullptr;

	ECharacterStates CharacterState = ECharacterStates::ECS_UnEquiped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionStates ActionState = EActionStates::EAS_Unoccupied;
	
	// 暴露给蓝图, 且允许蓝图修改, 将播放蒙太奇的决定权留给蓝图
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage = nullptr;
};
