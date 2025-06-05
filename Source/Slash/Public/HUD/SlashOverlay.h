// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);
	void SetStaminaPercent(float Percent);
	void SetCoinsText(int32 Coins);
	void SetExperiencePointsText(int32 Souls);
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProcessBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProcessBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinsText = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExperiencePointsText = nullptr;
};
