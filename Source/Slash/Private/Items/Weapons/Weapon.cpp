// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Slash/Public/Characters/SlashCharacter.h"
#include "Slash/DebugMacros.h"
#include "Enemy/Enemy.h"

#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "NiagaraComponent.h"
#include "GameFramework/DamageType.h"

AWeapon::AWeapon() {
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());

	// 配置 Collision 属性
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetGenerateOverlapEvents(true);

	//我这里采取了与示例不同的方案
	TraceStart = CreateDefaultSubobject<USceneComponent>(FName("TraceStart"));
	TraceStart->SetupAttachment(CollisionBox);
		
	TraceEnd = CreateDefaultSubobject<USceneComponent>(FName("TraceEnd"));
	TraceEnd->SetupAttachment(CollisionBox);
}

void AWeapon::EquipWeapon(USceneComponent* Inparent, const FName& InSocket, AActor* NewOwner, APawn* InInstigator) {
	AttachToComponentSnap(Inparent, InSocket);
	ItemState = EItemStates::EIS_Equipped;
	
	SetOwner(NewOwner); // 武器拥有者
	SetInstigator(InInstigator); // 物理世界行为发起者

	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation()); // 此处Actor指 Weapon 实例
	}
	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 捡到武器后其实碰撞球就没用了
		Sphere = nullptr; // 示例代码没有做这里
		if (ItemEffect) { // 捡到武器后就不需要效果了
			ItemEffect->Deactivate(); // 释放
		}
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
	auto PickupInterface = Cast<IPickupInterface>(OtherActor); // 父类转子类
	if (PickupInterface) { // 如果成功
		PickupInterface->SetOverlappingItem(this); // 这里是针对武器的, 
	}
}

void AWeapon::OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	auto PickupInterface = Cast<IPickupInterface>(OtherActor); // 父类转子类
	if (PickupInterface) { // 如果成功
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
	// 由于对武器的起始姿态做了调整, 因而直接在蓝图调整位置, 非常直观
	//// 对 TraceStart, TraceEnd 调整
	////这里Location 的确定需要根据蓝图中CollisionBox 的调整情况确定
	//auto Extent = CollisionBox->GetScaledBoxExtent();
	//TraceStart->SetRelativeLocation(FVector(0.f, -Extent.Y, 0.f));
	//TraceEnd->SetRelativeLocation(FVector(0.f, Extent.Y, 0.f));

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
	// 避免 weapon 的 collision box 与 weapon 自己的其它组件 overlap
	// 如果之前已被攻击过一次, 已经在 IgnoreActors 中了
	if (OtherActor == this || IgnoreActors.Find(OtherActor) != INDEX_NONE || IsHitPartner(OtherActor)) return;

	FHitResult HitResult;
	auto Extent = CollisionBox->GetScaledBoxExtent(); // InitialExten * Scale
	auto IsHit = UKismetSystemLibrary::BoxTraceSingle(this,
		TraceStart->GetComponentLocation(), TraceEnd->GetComponentLocation(),
		FVector(Extent.X * 0.5f, Extent.Y * 0.5f, 1.f), FRotator(0.f), ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>{this, GetOwner()},
		bDrawDebugTraceBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult, true);

	if (auto HitedActor = HitResult.GetActor(); HitedActor) {
		if (IsHitPartner(HitedActor)) return; // 即使overlap 的是玩家, trace 到的目标也可能为enemy
		IgnoreActors.AddUnique(HitedActor); // 只有当 IgnoreActors 不存在 HitedActor, 才会添加
		if (auto InInstigator = GetInstigator(); InInstigator) {
			if (auto Controller = InInstigator->GetController(); Controller) {
				UGameplayStatics::ApplyDamage(HitedActor, BaseDamage, Controller, this, UDamageType::StaticClass());
			}
		}
		// 基类转子类
		if (auto HitInterface = Cast<IHitInterface>(HitedActor); HitInterface) { 
			//Enemy->GetHited(HitResult.ImpactPoint);
			HitInterface->Execute_GetHited(HitedActor, HitResult.ImpactPoint, GetOwner());
		}
		// 我感觉在这里调用不是很合适, 直接在GetHited 中调用比较合适, 但是GetHited 是虚函数, 不知道是否有影响
		CreateField(HitResult.ImpactPoint);
	}
}

bool AWeapon::IsHitPartner(AActor* OtherActor) {
	//return Owner && Owner->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
	return GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
}
