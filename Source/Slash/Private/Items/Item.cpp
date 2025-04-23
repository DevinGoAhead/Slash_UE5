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

float AItem::TransformedSin() const{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
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

	DRAW_DEBUG_SPHERE(this, GetActorLocation(), 25.f, int32(24), FColor::Blue);
}

