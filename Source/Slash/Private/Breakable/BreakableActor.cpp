// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

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
	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(3, 5., FColor::Red, FString("ABreakableActor::BeginPlay()"));
	//}
}

