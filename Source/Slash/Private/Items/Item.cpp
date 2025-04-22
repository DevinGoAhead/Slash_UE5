// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"

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

	UWorld * World = GetWorld();
	if (World) {
		FVector ActorLocation = GetActorLocation();
		// Segment, ���� shape �����������
		// bPersistentLines, �� true, �����ô��ڲ���ʧ, ��ʱ LifeTime ����ʧЧ
		DrawDebugSphere(World, ActorLocation, 25.f, int32(32), FColor::Cyan, true);

		// ָ�� Item ���� ��λ������������, ����Ϊ��λ�������ȵ�100��
		// DepthPriority, ������ȼ�, �������ȼ�
		DrawDebugLine(World, ActorLocation, ActorLocation + GetActorForwardVector() * FVector(100.f),
					FColor::Yellow, true, -1.f, uint8(0U), 0.2f);
		DrawDebugPoint(World, ActorLocation, 50.f, FColor::Green, true); // ���
		DrawDebugPoint(World, ActorLocation + GetActorForwardVector() * FVector(100.f), 50.f, FColor::Green, true); // ���
	}
	
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

