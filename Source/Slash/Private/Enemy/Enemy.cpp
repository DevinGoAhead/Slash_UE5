// Fill out you copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Slash/DebugMacros.h"
#include "Slash/Public/Components/AttributeComponent.h"
#include "Slash/Public/HUD/HealthBarComponent.h"

#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

#include <type_traits>

// Sets default values
AEnemy::AEnemy(){
	PrimaryActorTick.bCanEverTick = true;

	auto SKMesh = GetMesh();
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 由于游戏中的Weapon 设置为忽略 Pawn, 因而这里不能设置 Pawn
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 忽略相机
	SKMesh->SetGenerateOverlapEvents(true); //启用生成overlap事件
	
	// 胶囊组件碰撞响应
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	// 不需要附加到组件, 因为没有mesh, 也没有位置或其它属性, 仅仅存储数据, 不需要显示
	Attributes = CreateDefaultSubobject<UAttributeComponent>(FName("Attributes"));

	// 与 Attributes 匹配, 但是这个是要实际显示的
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void AEnemy::Die() {
	// 死亡动画
	int32 index = FMath::RandRange(int32(1), int32(DeathMontage->CompositeSections.Num()));
	//ADD_SCREEN_DEBUG(2, FString::FromInt(index));
	DeathPose = static_cast<EDeathPose>(index); // UE Cast 转换失败, 故使用 c++ 库函数
	PlayMontage(FName("Death_" + FString::FromInt(index)), DeathMontage);

	// 后续处理
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 禁止胶囊碰撞
	SetLifeSpan(3.f); // 5s 后对象即销毁
}

void AEnemy::GetHited_Implementation(const FVector& Impactpoint) {
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
	if (Attributes) {
		if (Attributes->IsAlive() && HitedMontage) {
			DeathPose = EDeathPose::EDP_Alive;
			PlayMontage(FName("HitedReact_" + GetHitedDirection(Impactpoint)), HitedMontage);
		}
		else if (!Attributes->IsAlive() && DeathMontage){
			Die();
		}
	}
	if (HitedSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitedSound, Impactpoint);
	}
	if (HitedParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitedParticle, Impactpoint);
	}
}

FString AEnemy::GetHitedDirection(const FVector& Impactpoint) {
	FVector Forward = GetActorForwardVector();
	FVector ToHit = (Impactpoint - GetActorLocation()).GetSafeNormal(); // 归一化
	auto Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit))); // 点积反算角度

	auto CrossValue = FVector::CrossProduct(Forward, ToHit); // 叉积确定范围
	if (CrossValue.Z < 0.) {
		Theta *= -1.;
	}
	if (Theta >= -135. && Theta < -45.) { return "Left"; }
	else if (Theta >= -45. && Theta < 45.) { return "Front"; }
	else if (Theta >= 45. && Theta < 135.) { return "Right"; }
	else { return "Back"; }
}

void AEnemy::PlayMontage(FName MontageSction, UAnimMontage* Montage) {
	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(Montage);// 准备播放状态
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // 初始不可见
		HealthBarWidget->SetHealthPercent(1.f); // 初始化为100%
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 目标超过一定范围, 则放弃攻击并隐藏血条
	if (CombatTarget) {
		auto TargetLocation = CombatTarget->GetActorLocation();
		auto SelfLocation = GetActorLocation();
		auto DistanceToTarget = (TargetLocation - SelfLocation).Size();
		//ADD_SCREEN_DEBUG(3, FString::Printf(TEXT("DistanceToTarget: %f"), DistanceToTarget));
		if (DistanceToTarget > CombatRadius) {
			CombatTarget = nullptr;
			if (HealthBarWidget) {
				HealthBarWidget->SetVisibility(false);
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	CombatTarget = EventInstigator->GetPawn(); // 子类指针转父类, 切片
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (HealthBarWidget) {
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}
	// 这里暂先认为 Enemy 没有任何抗性, 全部伤害转换为真实伤害
	return  DamageAmount;
}

