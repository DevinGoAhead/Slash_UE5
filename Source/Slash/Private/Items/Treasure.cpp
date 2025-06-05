// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
//#include "Characters/SlashCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void ATreasure::OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (auto PickupInterface = Cast<IPickupInterface>(OtherActor); PickupInterface) { // 父类转子类
		//UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		SpawnPickupSound();
		PickupInterface->PickupTreasure(this);
		Destroy();
	}
}

void ATreasure::OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}