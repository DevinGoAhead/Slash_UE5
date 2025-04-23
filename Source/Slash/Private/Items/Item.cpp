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
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;
	AddActorWorldOffset(FVector(0.f, 0.f, Amplitude * FMath::Sin(RunningTime * TimeConstant)));// 世界坐标中位置的变换
	
	FVector ActorLocation = GetActorLocation();
	DRAW_DEBUG_SPHERE(this, ActorLocation, 25.f, int32(32), FColor::Cyan, false);
	
}

