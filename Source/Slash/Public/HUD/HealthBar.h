// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UProgressBar;
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar = nullptr; // 这与 WBP_HealthBar 中的 HealthBar 建立关联, 二者名字必须相同, 否则会崩溃
};
