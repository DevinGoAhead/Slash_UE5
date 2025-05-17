// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Slash/Public/Items/TreasureTypes.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetIndex(uint32 Index) { TreasureIndex = Index; }
	FORCEINLINE const TArray<FTreasureProperty>& GetTreasureProperties() { return TreasureProperties; }
protected:
	virtual void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;


	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
private:
	// 封装到 FTreasureProperty 中
	//UPROPERTY(EditAnywhere, Category = "TreasureProperty")
	//USoundBase* PickupSound;

	UPROPERTY(EditAnywhere)
	TArray<FTreasureProperty> TreasureProperties;
	uint32 TreasureIndex; // 爆出宝物的随机下标
};
