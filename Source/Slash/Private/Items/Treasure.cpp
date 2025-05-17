// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void ATreasure::OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) {
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor); // 父类转子类
	if (SlashCharacter) { // 如果成功
		if (TreasureProperties[TreasureIndex].PickupSound) {
			UGameplayStatics::PlaySoundAtLocation(this, TreasureProperties[TreasureIndex].PickupSound, GetActorLocation());
		}
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