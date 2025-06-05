// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveDamage(float DamageAmount);
	float GetHealthPercent() const;
	void UseStamina(float StaminaAmount);
	float GetStaminaPercent() const;
	void RegenStamina(float DeltaTime);
	void AddCoins(uint32 NumOfCoins);
	void AddExperiencePoints(uint32 NumOfExperiencePoints);
	FORCEINLINE bool IsAlive() const { return Health > 0.f; }
	FORCEINLINE uint32 GetCoins() const { return Coins; }
	FORCEINLINE uint32 GetExperiencePoints() const { return ExperiencePoints; }
	FORCEINLINE float GetStamina() const { return Stamina; }
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float MaxHealth = 100.f; // Ѫ�����ֵ

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float Health = MaxHealth; // ��ǰʵ��Ѫ��ֵ

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float MaxStamina = 100.f; // �������ֵ

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float Stamina = MaxStamina; // ��ǰ����ֵ
	
	// ������ʵӦ����ϸ���ּ�һ��, ��ΪEnemy �Ƚ�ɫ����Ҫ��Щ����
	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	uint32 Coins = 0; // ���

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	uint32 ExperiencePoints = 0; // ����ֵ
};
