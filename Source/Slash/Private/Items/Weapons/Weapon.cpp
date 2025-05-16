// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Slash/DebugMacros.h"
#include "Enemy/Enemy.h"

AWeapon::AWeapon() : EquipSound(nullptr) {
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());

	// 配置 Collision 属性
	//CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AWeapon::SetIgnoreActorsEmpty() {
	if (!IgnoreActors.IsEmpty()) IgnoreActors.Empty();
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
	if (OtherActor == this) return; // 避免 weapon 的 collision box 与 weapon 自己的其它组件 overlap
	if (IgnoreActors.Find(OtherActor) != INDEX_NONE) return; // 如果之前已被攻击过一次, 已经在 IgnoreActors 中了

	FHitResult HitResult;
	auto Extent = CollisionBox->GetScaledBoxExtent();
	auto IsHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStart->GetComponentLocation(),
		TraceEnd->GetComponentLocation(), FVector(Extent.X, 0.5f, Extent.Z), 
		TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, TArray<AActor*>{this}, EDrawDebugTrace::None, HitResult, true);

	auto HitedActor = HitResult.GetActor();
	if (HitedActor) {
		IgnoreActors.AddUnique(HitedActor); // 只有当 IgnoreActors 不存在 HitedActor, 才会添加
		auto HitInterface = Cast<IHitInterface>(HitedActor); // 基类转子类
		if (HitInterface) { //如果转换成功
			//Enemy->GetHited(HitResult.ImpactPoint);
			HitInterface->Execute_GetHited(HitedActor, HitResult.ImpactPoint);
		}
		// 我感觉在这里调用不是很合适, 直接在GetHited 中调用比较合适, 但是GetHited 是虚函数, 不知道是否有影响
		CreateField(HitResult.ImpactPoint);
	}
}
