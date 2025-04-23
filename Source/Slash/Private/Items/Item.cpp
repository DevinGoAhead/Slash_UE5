// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "../DebugMacros.h"

// Sets default values
AItem::AItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// ��� actor ����Ҫ���� tick, ���� ��̬����, ��������Ϊ false
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, ���ø����Ա��	��
	// ���ܸ��������һЩ����
	Super::BeginPlay();
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;
	AddActorWorldOffset(FVector(0.f, 0.f, Amplitude * FMath::Sin(RunningTime * TimeConstant)));// ����������λ�õı任
	
	FVector ActorLocation = GetActorLocation();
	DRAW_DEBUG_SPHERE(this, ActorLocation, 25.f, int32(32), FColor::Cyan, false);
	
}

