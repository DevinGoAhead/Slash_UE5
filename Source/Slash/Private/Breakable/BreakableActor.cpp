// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Slash/Public/Items/Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor() : bBroken(false) {
	PrimaryActorTick.bCanEverTick = false; // 不需要 tick
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(FName("GeometryComponent"));
	SetRootComponent(GeometryCollection);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName("CapsuleComponent"));
	Capsule->SetupAttachment(GeometryCollection);

	// 设置 collision
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionObjectType(ECollisionChannel::ECC_Destructible);
	GeometryCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic); // Echo 对 WorldStatic 的碰撞响应是 block
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHited_Implementation(const FVector& Impactpoint) {
	if (!bBroken){
		bBroken = true;
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		//GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		auto World = GetWorld();
		if (World && !TreasureClasses.IsEmpty()) {
			auto Location = GetActorLocation();
			Location.Z += 80;
			uint32 Index = FMath::RandRange(int32(0), int32(TreasureClasses.Num() - 1));
			auto SpawnedTreasure = World->SpawnActor<ATreasure>(TreasureClasses[Index], Location, GetActorRotation());
		}
	}	
}

