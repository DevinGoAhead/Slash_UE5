// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "../DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Slash/Public/Interfaces/PickupInterface.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
AItem::AItem() : TimeConstant(1.5f), Amplitude(1.f), ItemState(EItemStates::EIS_Hoverring){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 如果 actor 不需要参与 tick, 例如 静态对象, 可以设置为 false
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh; // 将 ItemMesh 设置为根组件

	// 创建组件
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(ItemMesh);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("EmbersEffect"));
	ItemEffect->SetupAttachment(ItemMesh);
	
	// 碰撞响应配置
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭mesh的碰撞响应

	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// 由于 角色的 capsule 为 pawn 且已忽略了 dynamic
	// 这里只能使用 角色的 mesh 与 sphere 触发overlap, 而mesh 的类型是 dynamic
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);
}

float AItem::TransformedSin() const {
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos() const{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult) {
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
}

void AItem::SpawnPickupSound() {
	if (PickupSound) {
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItem::SpawnPickupSystem() {
	if (PickupEffect) {
		auto Location = GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, FVector(Location.X, Location.Y, (Location.Z - 200)));
	}
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, 调用父类成员函	数
	// 可能父类完成了一些功能
	// 添加委托
	Super::BeginPlay();
	if (Sphere) {
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	if (ItemState == EItemStates::EIS_Hoverring) {
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

