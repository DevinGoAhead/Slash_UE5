// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Slash/DebugMacros.h"

AWeapon::AWeapon() : EquipSound(nullptr) {
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());

	// 配置 Collision 属性
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // 避免与角色碰撞

	//我这里采取了与示例不同的方案
	TraceStart = CreateDefaultSubobject<USceneComponent>(FName("TraceStart"));
	TraceStart->SetupAttachment(CollisionBox);
		
	TraceEnd = CreateDefaultSubobject<USceneComponent>(FName("TraceEnd"));
	TraceEnd->SetupAttachment(CollisionBox);
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

void AWeapon::BeginPlay() {
	Super::BeginPlay();
	// 对 TraceStart, TraceEnd 调整
	//这里Location 的确定需要根据蓝图中CollisionBox 的调整情况确定
	auto Extent = CollisionBox->GetScaledBoxExtent();
	TraceStart->SetRelativeLocation(FVector(0.f, -Extent.Y, 0.f));
	TraceEnd->SetRelativeLocation(FVector(0.f, Extent.Y, 0.f));
	/*DRAW_DEBUG_SPHERE(this, TraceStart->GetComponentLocation(), 2.5f, 10,
		FColor::Cyan, false, 50.f, (uint8)0U, 1.f);
	DRAW_DEBUG_SPHERE(this, TraceEnd->GetComponentLocation(), 2.5f, 10,
		FColor::Cyan, false, 50.f, (uint8)0U, 1.f);*/
	if (CollisionBox) {
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	}
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult){
	//这里主要在debug 一个问题, 当 组件的collision状态改变时, 会触发一次overlap 检测, 此时子组件会与父组件发生overlap
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(2, 10.f, FColor::Blue, FString(OtherActor->GetFName().ToString()));

	}
	FHitResult HitResult;
	auto Extent = CollisionBox->GetScaledBoxExtent();
	auto IsHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStart->GetComponentLocation(), TraceEnd->GetComponentLocation(),
		FVector(Extent.X, 0.5f, Extent.Z), TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, true, TArray<AActor*>{this}, EDrawDebugTrace::ForDuration,
		HitResult, true);
	DRAW_DEBUG_SPHERE(this, HitResult.ImpactPoint, 3.f, 5,
		FColor::Cyan, false, 10.f, (uint8)0U, 1.f);
	if (IsHit) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Blue, FString(HitResult.GetActor()->GetFName().ToString()));

		}
	}
	else{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(4, 10.f, FColor::Blue, FString("HitNothing"));

		}
	}
}
