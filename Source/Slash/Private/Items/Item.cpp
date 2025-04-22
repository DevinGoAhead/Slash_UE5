// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "../DebugMacros.h"

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
	FVector ActorLocation = GetActorLocation();

	DRAW_DEBUG_SPHERE(this, ActorLocation, 25.f, int32(32), FColor::Cyan, true);
	DRAW_DEBUG_LINE(this, ActorLocation, ActorLocation + GetActorForwardVector() * FVector(100.f),
		FColor::Yellow, true, -1.f, uint8(0U), 0.2f);

	DRAW_DEBUG_POINT(this, ActorLocation, 50.f, FColor::Green, true); // 起点
	DRAW_DEBUG_POINT(this, ActorLocation + GetActorForwardVector() * FVector(100.f), 50.f, FColor::Green, true); // 起点
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

