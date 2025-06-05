// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Treasure.h"
#include "Items/Soul.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Slash/Public/Components/AttributeComponent.h"
#include "Slash/DebugMacros.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/BoxComponent.h"

#include "Animation/AnimMontage.h"

// Sets default values
ASlashCharacter::ASlashCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	auto CharactorRootComponent = GetRootComponent();

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SetupAttachment(CharactorRootComponent);

	CameraView = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraView"));
	CameraView->SetupAttachment(SpringArm);

	//添加毛发组件到 mesh 上
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // 绑定插槽

	Eyebrow = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrow"));
	Eyebrow->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // 绑定插槽
}

void ASlashCharacter::BeginPlay() {
	Super::BeginPlay();
	// 用于判定 Enemy::OnPawnSee 看到的是玩家而不是其它 Enemy
	// 原始方案是每一次都将看到的角色 CastToSlashCharacter, 根据转换结果判断是否是SlashCharacter, 这样将更消耗资源
	Tags.Add(FName("EngageableTarget"));

	if (auto SlashController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()); SlashController) {
		if (auto SlashHUD = Cast<ASlashHUD>(SlashController->GetHUD()); SlashHUD) {
			if (SlashOverlay = SlashHUD->GetSlashOverlay(); SlashOverlay && Attributes) {
				SlashOverlay->SetHealthPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaPercent(1.f);
				SlashOverlay->SetCoinsText(Attributes->GetCoins());
				SlashOverlay->SetExperiencePointsText(Attributes->GetExperiencePoints());
			}
		}
	}
}

// 将武器挂载到 SpineSocket
void ASlashCharacter::Disarm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("SpineSocket"));
	}
}

// 将武器挂载到 RightHandSocket
void ASlashCharacter::Arm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::MoveForward(float Value) {
	//if (ActionState == EActionStates::EAS_Dead || ActionState != EActionStates::EAS_Unoccupied || !Controller || !Value) return;
	// 相机的 forward 向量 会随着控制器旋转, 这个旋转可能会导致 forward 向下向上倾斜, 而我们只希望获得 XZ 平面的分量
	// 相机的的旋转与 Controller 是一致的, 而我们需要的forward XZ 平面的分量本质就是 Controller 旋转矩阵中受 Yaw 影响的部分
	// 可以理解为XZ 平面的一个单位向量, 累计转动 Yaw 之后的方向, 即 (cos(Yaw), sin(Yaw), 0.f)
	// Direction = (cos(Yaw), sin(Yaw), 0.f) // Yaw 要转为弧度
	// 示例代码中使用了一种更稳妥的方式而已
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		const FRotator ControlRotation = GetControlRotation(); // 控制器的欧拉角结构体
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//基于 Yaw 构造旋转矩阵
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Value);
	}
}

void ASlashCharacter::MoveRight(float Value) {
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		// 本质是 Direction = (cos(Yaw + π / 2), sin(Yaw +  π / 2), 0.f) // Yaw 要转为弧度
		const FRotator ControlRotation = GetControlRotation(); // 控制器的欧拉角结构体
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//基于 Yaw 构造旋转矩阵
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Value);
	}
}

void ASlashCharacter::Jump() {
	if (ActionState == EActionStates::EAS_Unoccupied) {
		Super::Jump();
	}
}

void ASlashCharacter::Dodge() {
	if (ActionState == EActionStates::EAS_Unoccupied && DodgeMontage
			&& Attributes && Attributes->GetStamina() > StaminaCostOfDodge) {
		Super::Dodge();
		//ADD_SCREEN_DEBUG(9, "ASlashCharacter::Dodge() ");
		ActionState = EActionStates::EAS_Dodging;
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Attributes->UseStamina(StaminaCostOfDodge);
		PlayMontage("Default", DodgeMontage);
	}
}

void ASlashCharacter::DodgeEnd() {
	Super::DodgeEnd();
	ActionState = EActionStates::EAS_Unoccupied;
	//ADD_SCREEN_DEBUG(8, "ASlashCharacter::DodgeEnd() ");
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASlashCharacter::Turn(float Value) {
	if (Controller && Value) {
		AddControllerYawInput(Value);
	}
}

void ASlashCharacter::LookUp(float Value) {
	if (Controller && Value) {
		AddControllerPitchInput(Value);
	}
}

void ASlashCharacter::EKeyPressed() {
	if (ActionState == EActionStates::EAS_Dead) return;
	if (OverlappingItem) {
		EquippedWeapon = Cast<AWeapon>(OverlappingItem);
		if (EquippedWeapon) {
			// 第 2 个 this 为 InInstigator, 不同于EventInstigator
			EquippedWeapon->EquipWeapon(GetMesh(), FName("RightHandSocket"), this, this);
			CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon;
			OverlappingItem = nullptr;
		}
	}
	
	// 空闲状态 且 设置了有效的 Montage, 允许装卸武器
	else if (EquippedWeapon && ActionState == EActionStates::EAS_Unoccupied && EquipMontage){
		if (CharacterState != ECharacterStates::ECS_UnEquiped) { //此时处于装备武器状态, 即允许执行卸载武器动作
			ActionState = EActionStates::EAS_Equipping;
			PlayMontage(FName("Disarm"), EquipMontage);
			CharacterState = ECharacterStates::ECS_UnEquiped; // 卸载武器后设置为未装备状态
		}
		else if (CharacterState == ECharacterStates::ECS_UnEquiped) {
			ActionState = EActionStates::EAS_Equipping;
			PlayMontage(FName("Arm"), EquipMontage);
			CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon; // 装备武器后设置为装备状态
		}
	}
}

void ASlashCharacter::PickupTreasure(ATreasure* Treasure) {
	if (Attributes && SlashOverlay) {
		Attributes->AddCoins(Treasure->GetCoinValue());
		SlashOverlay->SetCoinsText(Attributes->GetCoins());
	}
}

void ASlashCharacter::PickupSoul(ASoul* Soul) {
	if (Attributes && SlashOverlay) {
		Attributes->AddExperiencePoints(Soul->GetExperiencePointValue());
		SlashOverlay->SetExperiencePointsText(Attributes->GetExperiencePoints());
	}
}

void ASlashCharacter::Attack() {
	Super::Attack();
	//UE_LOG(LogTemp, Warning, TEXT("After Super::Attack();"));

	if (ActionState == EActionStates::EAS_Unoccupied
			&& CharacterState != ECharacterStates::ECS_UnEquiped
			&& AttackMontage) {
		ActionState = EActionStates::EAS_Attacking;
		//UE_LOG(LogTemp, Warning, TEXT("Befor PlayMontage"));
		PlayMontage(RandomMontageSection("Attack_", AttackMontage), AttackMontage);
	}
}

void ASlashCharacter::AttackEnd() {
	ActionState = EActionStates::EAS_Unoccupied;
}

void ASlashCharacter::Die() {
	Super::Die();
	ActionState = EActionStates::EAS_Dead;
	Tags.Add(FName("Dead"));
}

void ASlashCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	/*FString CurState = StaticEnum<EActionStates>()->GetNameStringByValue(static_cast<int8>(ActionState));
	ADD_SCREEN_DEBUG(6, FString::Printf(TEXT("EchoActionState: %s"), *CurState));

	FString CurState_ = StaticEnum<ECharacterStates>()->GetNameStringByValue(static_cast<int8>(CharacterState));
	ADD_SCREEN_DEBUG(7, FString::Printf(TEXT("EchoCharacterState: %s"), *CurState_));*/
	if (SlashOverlay && Attributes) {
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASlashCharacter::MoveRight);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName("EKeyPressed"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter::Dodge);
}

void ASlashCharacter::SetOverlappingItem(AItem* Item) {
	OverlappingItem = Item;
}

float ASlashCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser) {
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
		if (SlashOverlay && Attributes) {
			SlashOverlay->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}
	return DamageAmount;
}

void ASlashCharacter::GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) {
	ResetAttackState(); // 避免受到攻击后, 动画被打断, 无法执行
	Super::GetHited_Implementation(Impactpoint, Hitter);
}
