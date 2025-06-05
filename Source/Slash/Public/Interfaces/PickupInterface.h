// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AItem;
class ATreasure;
class ASoul;

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class SLASH_API IPickupInterface
{
	GENERATED_BODY()
public:
	virtual void SetOverlappingItem(AItem* Item);
	virtual void PickupTreasure(ATreasure* Treasure);
	virtual void PickupSoul(ASoul* Soul);
protected:

};
