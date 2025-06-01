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

	// ���� Collision ����
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetGenerateOverlapEvents(true);

	//�������ȡ����ʾ����ͬ�ķ���
	TraceStart = CreateDefaultSubobject<USceneComponent>(FName("TraceStart"));
	TraceStart->SetupAttachment(CollisionBox);
		
	TraceEnd = CreateDefaultSubobject<USceneComponent>(FName("TraceEnd"));
	TraceEnd->SetupAttachment(CollisionBox);
}

void AWeapon::EquipWeapon(USceneComponent* Inparent, const FName& InSocket, AActor* NewOwner, APawn* InInstigator) {
	AttachToComponentSnap(Inparent, InSocket);
	ItemState = EItemStates::EIS_Equipped;
	
	SetOwner(NewOwner); // ����ӵ����
	SetInstigator(InInstigator); // ����������Ϊ������

	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation()); // �˴�Actorָ Weapon ʵ��
	}
	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // ����������ʵ��ײ���û����
		Sphere = nullptr; // ʾ������û��������
		if (EmbersEffecct) { // ��������Ͳ���ҪЧ����
			EmbersEffecct->Deactivate(); // �ͷ�
		}
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
	// ���ڶ���������ʼ��̬���˵���, ���ֱ������ͼ����λ��, �ǳ�ֱ��
	//// �� TraceStart, TraceEnd ����
	////����Location ��ȷ����Ҫ������ͼ��CollisionBox �ĵ������ȷ��
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
	// ���� weapon �� collision box �� weapon �Լ���������� overlap
	// ���֮ǰ�ѱ�������һ��, �Ѿ��� IgnoreActors ����
	if (OtherActor == this || IgnoreActors.Find(OtherActor) != INDEX_NONE || IsHitPartner(OtherActor)) return;

	FHitResult HitResult;
	auto Extent = CollisionBox->GetScaledBoxExtent(); // InitialExten * Scale
	auto IsHit = UKismetSystemLibrary::BoxTraceSingle(this,
		TraceStart->GetComponentLocation(), TraceEnd->GetComponentLocation(),
		FVector(Extent.X * 0.5f, Extent.Y * 0.5f, 1.f), FRotator(0.f), ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>{this, GetOwner()},
		bDrawDebugTraceBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult, true);

	if (auto HitedActor = HitResult.GetActor(); HitedActor) {
		if (IsHitPartner(HitedActor)) return; // ��ʹoverlap �������, trace ����Ŀ��Ҳ����Ϊenemy
		IgnoreActors.AddUnique(HitedActor); // ֻ�е� IgnoreActors ������ HitedActor, �Ż����
		if (auto InInstigator = GetInstigator(); InInstigator) {
			if (auto Controller = InInstigator->GetController(); Controller) {
				UGameplayStatics::ApplyDamage(HitedActor, BaseDamage, Controller, this, UDamageType::StaticClass());
			}
		}
		// ����ת����
		if (auto HitInterface = Cast<IHitInterface>(HitedActor); HitInterface) { 
			//Enemy->GetHited(HitResult.ImpactPoint);
			HitInterface->Execute_GetHited(HitedActor, HitResult.ImpactPoint, GetOwner());
		}
		// �Ҹо���������ò��Ǻܺ���, ֱ����GetHited �е��ñȽϺ���, ����GetHited ���麯��, ��֪���Ƿ���Ӱ��
		CreateField(HitResult.ImpactPoint);
	}
}

bool AWeapon::IsHitPartner(AActor* OtherActor) {
	//return Owner && Owner->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
	return GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
}
