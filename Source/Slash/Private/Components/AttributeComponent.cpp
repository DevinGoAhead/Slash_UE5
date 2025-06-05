// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::ReceiveDamage(float DamageAmount) {
	Health -= DamageAmount;
	if (Health < 0.f) {
		Health = 0.f;
	}
}

float UAttributeComponent::GetHealthPercent() const {
	if (MaxHealth > 0.f) {
		return Health / MaxHealth;
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString(TEXT("MaxHealth <= 0.f")));
		}
		return 0.f;
	}
}

void UAttributeComponent::UseStamina(float StaminaAmount) {
	Stamina -= StaminaAmount;
	if (Stamina < 0.f) {
		Stamina = 0.f;
	}
}

float UAttributeComponent::GetStaminaPercent() const {
	if (MaxStamina > 0.f) {
		return Stamina / MaxStamina;
	}
	return 0.0f;
}

void UAttributeComponent::RegenStamina(float DeltaTime) {
	Stamina += DeltaTime;
	if (Stamina > MaxStamina) {
		Stamina = MaxStamina;
	}
}

void UAttributeComponent::AddCoins(uint32 NumOfCoins) {
	Coins += NumOfCoins;
}

void UAttributeComponent::AddExperiencePoints(uint32 NumOfExperiencePoints) {
	ExperiencePoints += NumOfExperiencePoints;
}

