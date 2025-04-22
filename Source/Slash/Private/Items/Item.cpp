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
	
	// ����ʾ���е�����������һЩ, MoveRate, RoationRate
	float RotationSpeed = 45.f;
	float MoveSpeed = 40.f; // cm / s
	
	// ÿ��Ⱦһ֡����һ��Tick, �൱��ÿ֡��ʱ�� DeltaTime
	// ÿ֡���ٶ� MoveSpeed *  DeltaTime, �����ÿ����ٶ�  MoveSpeed *  DeltaTime * ֡/s = MoveSpeed *  DeltaTime * 1/DeltaTime =  MoveSpeed
	// �������ٶ���֡��(Ӳ��)����
	AddActorWorldRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f)); // ��Ȼ��world, ��������Ծֲ�����ϵԭ�����ת, ƫ����, ������, ������
	AddActorWorldOffset(FVector(MoveSpeed, 0.f, 0.f) * DeltaTime);// ����������λ�õı任
	
	FVector ActorLocation = GetActorLocation();
	DRAW_DEBUG_SPHERE(this, ActorLocation, 25.f, int32(32), FColor::Cyan, false);
	DRAW_DEBUG_LINE(this, ActorLocation, ActorLocation + GetActorForwardVector() * FVector(50.f),
		FColor::Yellow, false, -1.f, uint8(0U), 0.1f);

	DRAW_DEBUG_POINT(this, ActorLocation, 50.f, FColor::Green, false); // ���
	DRAW_DEBUG_POINT(this, ActorLocation + GetActorForwardVector() * FVector(100.f), 50.f, FColor::Green, false); // �յ�
}

