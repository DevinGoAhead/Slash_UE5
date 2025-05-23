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
	FORCEINLINE bool IsAlive() const { return Health > 0.f; }
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float MaxHealth = 100.f; // 血条最大值

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float Health = MaxHealth; // 当前实际血条值
};
