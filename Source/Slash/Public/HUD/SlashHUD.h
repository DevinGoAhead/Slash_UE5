// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class USlashOverlay;
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()
public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() { return SlashOverlay; }
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	USlashOverlay* SlashOverlay;
};
