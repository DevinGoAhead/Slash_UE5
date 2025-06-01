// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UGeometryCollectionComponent;
class UCapsuleComponent;
//class FChaosBreakEvent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnIndirectlyBroken(const FChaosBreakEvent& BreakEvent);
private:
	void SpawnActor();
private:
	UPROPERTY(VisibleAnywhere);
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "PickupClasses")
	/* UClass* TreasureClass //这种形式会导致蓝图 Detail 列表中可供选择的将是所有继承自UClass 的类 */
	TArray<TSubclassOf<class ATreasure>> TreasureClasses; // 这样将限定为 ATreasure 及其子类

	// 问题: GetHited 被多次调用, 导致无限循环, 堆栈溢出
	bool bBroken;

	// 问题, 物体打碎后, 碎片会 block 玩家角色
	// 直接GeometryCollection 忽略pawn, 然后添加一个胶囊, 仅对 Pawn 碰撞响应 block, 打碎后再忽略Pawn, 示例使用的这个方案
	// 可是为什么不直接打碎后直接 GeometryCollection 忽略pawn?
	// // // 这个方案我测试了, 发现不行, 虽然打碎后, 碎片的 Detail 中看到碎片对pawn的碰撞响应确实是ignore, 
	// // // 但实际碎片与pawn 也确实发生了 collision, 比如碎片会与角色移动碰撞发生飞起, 旋转
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;
};
