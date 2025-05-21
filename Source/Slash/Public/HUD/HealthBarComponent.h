// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UHealthBar;

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);
private:
	UHealthBar* HealthBarWidget = nullptr; // UHealthBar* HealthBarWidget, UHealthBar¼Ì³Ð×Ô UUserWidget
};
