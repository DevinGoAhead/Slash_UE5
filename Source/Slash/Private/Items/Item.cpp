// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "../DebugMacros.h"

// Sets default values
AItem::AItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 如果 actor 不需要参与 tick, 例如 静态对象, 可以设置为 false
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh; // 将 ItemMesh 设置为根组件
}

float AItem::TransformedSin() const{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos() const{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
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
}

