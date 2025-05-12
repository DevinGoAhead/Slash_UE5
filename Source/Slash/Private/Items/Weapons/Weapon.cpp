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

	// ���� Collision ����
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // �������ɫ��ײ

	//�������ȡ����ʾ����ͬ�ķ���
	TraceStart = CreateDefaultSubobject<USceneComponent>(FName("TraceStart"));
	TraceStart->SetupAttachment(CollisionBox);
		
	TraceEnd = CreateDefaultSubobject<USceneComponent>(FName("TraceEnd"));
	TraceEnd->SetupAttachment(CollisionBox);
}

void AWeapon::EquipWeapon(USceneComponent* Inparent, const FName& InSocket) {
	AttachToComponentSnap(Inparent, InSocket);
	ItemState = EItemStates::EIS_Equipped;
	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation()); // �˴�Actorָ Weapon ʵ��
	}
	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // ����������ʵ��ײ���û����
		Sphere = nullptr; // ʾ������û��������
	}
}

// ���߶���������˷�װ, �����Ҹо���Ҫ�Բ��Ǻܴ�
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
	// �� TraceStart, TraceEnd ����
	//����Location ��ȷ����Ҫ������ͼ��CollisionBox �ĵ������ȷ��
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
	//������Ҫ��debug һ������, �� �����collision״̬�ı�ʱ, �ᴥ��һ��overlap ���, ��ʱ��������븸�������overlap
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
