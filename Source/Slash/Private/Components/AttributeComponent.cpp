// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent() : MaxHealth(100.f), Health(MaxHealth) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttributeComponent::ReceiveDamage(float DamageAmount) {
	Health -= DamageAmount;
	if (Health < 0.f) {
		Health = 0.f;
		//...
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

