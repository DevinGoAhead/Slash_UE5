// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Characters/SlashCharacter.h"

void ASoul::OnSphereBeginOverlap(
		UPrimitiveComponent * OverlappedComponent,
		AActor * OtherActor,
		UPrimitiveComponent * OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (auto PickupInterface = Cast<IPickupInterface>(OtherActor); PickupInterface) {
		SpawnPickupSound();
		SpawnPickupSystem();
		PickupInterface->PickupSoul(this);
		Destroy();
	}
}
