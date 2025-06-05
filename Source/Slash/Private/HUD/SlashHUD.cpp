// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay() {
	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController(); Controller && SlashOverlayClass) {
		if (SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass); SlashOverlay) {
			SlashOverlay->AddToViewport();
		}
	}
}
