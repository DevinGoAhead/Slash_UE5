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
	
	// 好像示例中的命名更合适一些, MoveRate, RoationRate
	float RotationSpeed = 45.f;
	float MoveSpeed = 40.f; // cm / s
	
	// 每渲染一帧调用一次Tick, 相当于每帧的时间 DeltaTime
	// 每帧的速度 MoveSpeed *  DeltaTime, 换算回每秒的速度  MoveSpeed *  DeltaTime * 帧/s = MoveSpeed *  DeltaTime * 1/DeltaTime =  MoveSpeed
	// 本质是速度与帧率(硬件)解耦
	AddActorWorldRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f)); // 虽然有world, 但仍是相对局部坐标系原点的旋转, 偏航角, 俯仰角, 滚动角
	AddActorWorldOffset(FVector(MoveSpeed, 0.f, 0.f) * DeltaTime);// 世界坐标中位置的变换
	
	FVector ActorLocation = GetActorLocation();
	DRAW_DEBUG_SPHERE(this, ActorLocation, 25.f, int32(32), FColor::Cyan, false);
	DRAW_DEBUG_LINE(this, ActorLocation, ActorLocation + GetActorForwardVector() * FVector(50.f),
		FColor::Yellow, false, -1.f, uint8(0U), 0.1f);

	DRAW_DEBUG_POINT(this, ActorLocation, 50.f, FColor::Green, false); // 起点
	DRAW_DEBUG_POINT(this, ActorLocation + GetActorForwardVector() * FVector(100.f), 50.f, FColor::Green, false); // 终点
}

