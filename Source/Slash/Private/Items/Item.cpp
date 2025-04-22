// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

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

	// ����� UE output Log
	// UE_LOG() ��
	// ��־����, ��ϸ�ȼ�
	// TEXT(), ��, �ַ���������ת��Ϊ Unicode
	UE_LOG(LogTemp, Warning, TEXT("AItem::BeginPlay() called!"));

	// �����UE����
	if (GEngine) {
		// key, ����ͼ��key ������ͬ, ͬ����keyֵ��ǵ�������ݻᱻ����
		// TimeToDisplay, DeBugMessage ����ʾ����ʧ��ʱ��, ��ͬkey ����Ϣ�����Ǻ�, TimeToDisplay ����
		// bNewerOnTop, �µ���Ϣ����ʾ��������Ϣ���Ϸ�, ֻ�е� key = INDEX_NONE ʱ, bNewerOnTop �Ż���Ч
		GEngine->AddOnScreenDebugMessage(3, 25.f, FColor::Blue, FString("AItem::Begin() output in screnn!"));
	}
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// ��ȡ class Item ʵ�������������
	// ���罫һ�� class ���� place ����Ϸ��, Ȼ�� alt ����һ��, �����ǲ�ͬ������, ���ǻ���ø��Ե� AItem::Tick
	// ����������ĵ�����������������ͬ������
	FString ItemName = GetName();

	// Printf �ɽ����ݸ�ʽ��, ���� sprintf
	// *name, ���� c �ַ���, FString ������*, UE �и�ʽ�� FString ����ʹ��*
	FString OutPut = FString::Printf(TEXT("Item Name:, % s, Tick Time: % f"), *ItemName, DeltaTime);


	UE_LOG(LogTemp, Warning, TEXT("%s"), *OutPut);
	UE_LOG(LogTemp, Warning, TEXT("%2f"), DeltaTime); // һ���������, ����  TEXT
	GEngine->AddOnScreenDebugMessage(1, 25.f, FColor::Magenta, OutPut);
}

