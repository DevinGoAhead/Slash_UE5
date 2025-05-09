// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "../DebugMacros.h"

// Sets default values
AItem::AItem() : TimeConstant(4.f), Amplitude(10.f), ItemState(EItemStates::EIS_Hoverring){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// ��� actor ����Ҫ���� tick, ���� ��̬����, ��������Ϊ false
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh; // �� ItemMesh ����Ϊ�����

	// �������
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

float AItem::TransformedSin() const {
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos() const{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex,
	bool bFromSweep, 
	const FHitResult& SweepResult) {
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor); // ����ת����
	if (SlashCharacter) { // ����ɹ�
		SlashCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor); // ����ת����
	if (SlashCharacter) { // ����ɹ�
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}
// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, ���ø����Ա��	��
	// ���ܸ��������һЩ����
	Super::BeginPlay();
	if (Sphere) {
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	if (ItemState == EItemStates::EIS_Hoverring) {
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

