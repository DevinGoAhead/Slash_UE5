// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/Weapon.h"
#include "Slash/Public/Characters/SlashCharacter.h"

void AWeapon::OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor); // ����ת����
	if (SlashCharacter) { // ����ɹ�
		//SlashCharacter->GetMesh(),// SkeletalMeshComponent*
		
		ItemMesh->AttachToComponent(SlashCharacter->GetMesh(), 
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("RightHandSocket"));
	}
}

void AWeapon::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}