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

	// ���� Collision ����
	//CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	// �� TraceStart, TraceEnd ����
	//����Location ��ȷ����Ҫ������ͼ��CollisionBox �ĵ������ȷ��
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
	if (OtherActor == this) return; // ���� weapon �� collision box �� weapon �Լ���������� overlap
	if (IgnoreActors.Find(OtherActor) != INDEX_NONE) return; // ���֮ǰ�ѱ�������һ��, �Ѿ��� IgnoreActors ����

	FHitResult HitResult;
	auto Extent = CollisionBox->GetScaledBoxExtent();
	auto IsHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStart->GetComponentLocation(),
		TraceEnd->GetComponentLocation(), FVector(Extent.X, 0.5f, Extent.Z), 
		TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, TArray<AActor*>{this}, EDrawDebugTrace::None, HitResult, true);

	auto HitedActor = HitResult.GetActor();
	if (HitedActor) {
		IgnoreActors.AddUnique(HitedActor); // ֻ�е� IgnoreActors ������ HitedActor, �Ż����
		auto HitInterface = Cast<IHitInterface>(HitedActor); // ����ת����
		if (HitInterface) { //���ת���ɹ�
			//Enemy->GetHited(HitResult.ImpactPoint);
			HitInterface->Execute_GetHited(HitedActor, HitResult.ImpactPoint);
		}
		// �Ҹо���������ò��Ǻܺ���, ֱ����GetHited �е��ñȽϺ���, ����GetHited ���麯��, ��֪���Ƿ���Ӱ��
		CreateField(HitResult.ImpactPoint);
	}
}
