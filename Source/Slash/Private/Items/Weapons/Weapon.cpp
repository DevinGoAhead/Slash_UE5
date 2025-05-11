// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon() : EquipSound(nullptr) {
	Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	Box->SetupAttachment(GetRootComponent());
}

void AWeapon::EquipWeapon(USceneComponent* Inparent, const FName& InSocket) {
	AttachToComponentSnap(Inparent, InSocket);
	ItemState = EItemStates::EIS_Equipped;
	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation()); // 此处Actor指 Weapon 实例
	}
	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 捡到武器后其实碰撞球就没用了
		Sphere = nullptr; // 示例代码没有做这里
	}
}

// 作者对这里进行了封装, 但是我感觉必要性不是很大
void AWeapon::AttachToComponentSnap(USceneComponent* Inparent, const FName& InSocket) {
	AttachToComponent(Inparent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), InSocket);
}

void AWeapon::OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}