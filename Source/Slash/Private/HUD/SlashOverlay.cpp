// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Slash/DebugMacros.h"

void USlashOverlay::SetHealthPercent(float Percent) {
	if (HealthProcessBar) HealthProcessBar->SetPercent(Percent);
}

void USlashOverlay::SetStaminaPercent(float Percent) {
	if (StaminaProcessBar) StaminaProcessBar->SetPercent(Percent);
}

void USlashOverlay::SetCoinsText(int32 Coins) {
	if (CoinsText) CoinsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Coins)));
}

void USlashOverlay::SetExperiencePointsText(int32 Souls) {
	if (ExperiencePointsText) ExperiencePointsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
}

