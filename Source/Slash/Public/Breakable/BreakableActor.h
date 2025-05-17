// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UGeometryCollectionComponent;
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere);
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "PickupClass")
	///* UClass* TreasureClass 这种形式会导致蓝图 Detail 列表中可供选择的将是所有继承自UClass 的类 */
	//TArray<TSubclassOf<class ATreasure>> TreasureClasses; // 这样将限定为 ATreasure 及其子类
	TSubclassOf<class ATreasure> TreasureClass;
};
