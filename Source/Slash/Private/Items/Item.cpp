// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 如果 actor 不需要参与 tick, 例如 静态对象, 可以设置为 false
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, 调用父类成员函	数
	// 可能父类完成了一些功能
	Super::BeginPlay();

	UWorld * World = GetWorld();
	if (World) {
		FVector ActorLocation = GetActorLocation();
		// Segment, 构成 shape 的网格的数量
		// bPersistentLines, 若 true, 则永久存在不消失, 此时 LifeTime 参数失效
		DrawDebugSphere(World, ActorLocation, 25.f, int32(32), FColor::Cyan, true);

		// 指向 Item 对象 单位方向向量方向, 长度为单位向量长度的100倍
		// DepthPriority, 深度优先级, 覆盖优先级
		DrawDebugLine(World, ActorLocation, ActorLocation + GetActorForwardVector() * FVector(100.f),
					FColor::Yellow, true, -1.f, uint8(0U), 0.2f);
		DrawDebugPoint(World, ActorLocation, 50.f, FColor::Green, true); // 起点
		DrawDebugPoint(World, ActorLocation + GetActorForwardVector() * FVector(100.f), 50.f, FColor::Green, true); // 起点
	}
	
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

