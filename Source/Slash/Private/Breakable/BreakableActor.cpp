// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Slash/Public/Items/TreasureTypes.h"
#include "Slash/Public/Items/Treasure.h"

// Sets default values
ABreakableActor::ABreakableActor() {
	PrimaryActorTick.bCanEverTick = false; // 不需要 tick
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(FName("GeometryComponent"));
	SetRootComponent(GeometryCollection);

	// 设置 collision
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionObjectType(ECollisionChannel::ECC_Destructible);
	GeometryCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
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
	auto World = GetWorld();
	if (World) {
		if (auto DefaultTreasure = TreasureClass.GetDefaultObject()) {
			const auto& TreasureProps = DefaultTreasure->GetTreasureProperties();
			if (TreasureProps.IsEmpty()) {
				auto Location = GetActorLocation();
				Location.Z += 60;
				auto SpawnedTreasure = World->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());
				if (SpawnedTreasure) {
					uint32 Index = FMath::RandRange(int32(0), int32(TreasureProps.Num() - 1));
					//const FTreasureProperty& CurTreasureProperty = DefaultTreasure->
				}
			}
		}
		
		
	}
}

