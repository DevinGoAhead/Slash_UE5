// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "../DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Slash/Public/Interfaces/PickupInterface.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
AItem::AItem() : TimeConstant(1.5f), Amplitude(1.f), ItemState(EItemStates::EIS_Hoverring){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// ��� actor ����Ҫ���� tick, ���� ��̬����, ��������Ϊ false
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh; // �� ItemMesh ����Ϊ�����

	// �������
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(ItemMesh);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("EmbersEffect"));
	ItemEffect->SetupAttachment(ItemMesh);
	
	// ��ײ��Ӧ����
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//�ر�mesh����ײ��Ӧ

	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// ���� ��ɫ�� capsule Ϊ pawn ���Ѻ����� dynamic
	// ����ֻ��ʹ�� ��ɫ�� mesh �� sphere ����overlap, ��mesh �������� dynamic
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);
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
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
}

void AItem::SpawnPickupSound() {
	if (PickupSound) {
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItem::SpawnPickupSystem() {
	if (PickupEffect) {
		auto Location = GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, FVector(Location.X, Location.Y, (Location.Z - 200)));
	}
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, ���ø����Ա��	��
	// ���ܸ��������һЩ����
	// ���ί��
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

