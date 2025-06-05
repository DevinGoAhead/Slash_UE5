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

	//���ë������� mesh ��
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // �󶨲��

	Eyebrow = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrow"));
	Eyebrow->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // �󶨲��
}

void ASlashCharacter::BeginPlay() {
	Super::BeginPlay();
	// �����ж� Enemy::OnPawnSee ����������Ҷ��������� Enemy
	// ԭʼ������ÿһ�ζ��������Ľ�ɫ CastToSlashCharacter, ����ת������ж��Ƿ���SlashCharacter, ��������������Դ
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

// ���������ص� SpineSocket
void ASlashCharacter::Disarm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("SpineSocket"));
	}
}

// ���������ص� RightHandSocket
void ASlashCharacter::Arm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::MoveForward(float Value) {
	//if (ActionState == EActionStates::EAS_Dead || ActionState != EActionStates::EAS_Unoccupied || !Controller || !Value) return;
	// ����� forward ���� �����ſ�������ת, �����ת���ܻᵼ�� forward ����������б, ������ֻϣ����� XZ ƽ��ķ���
	// ����ĵ���ת�� Controller ��һ�µ�, ��������Ҫ��forward XZ ƽ��ķ������ʾ��� Controller ��ת�������� Yaw Ӱ��Ĳ���
	// �������ΪXZ ƽ���һ����λ����, �ۼ�ת�� Yaw ֮��ķ���, �� (cos(Yaw), sin(Yaw), 0.f)
	// Direction = (cos(Yaw), sin(Yaw), 0.f) // Yaw ҪתΪ����
	// ʾ��������ʹ����һ�ָ����׵ķ�ʽ����
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		const FRotator ControlRotation = GetControlRotation(); // ��������ŷ���ǽṹ��
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//���� Yaw ������ת����
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Value);
	}
}

void ASlashCharacter::MoveRight(float Value) {
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		// ������ Direction = (cos(Yaw + �� / 2), sin(Yaw +  �� / 2), 0.f) // Yaw ҪתΪ����
		const FRotator ControlRotation = GetControlRotation(); // ��������ŷ���ǽṹ��
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//���� Yaw ������ת����
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
			// �� 2 �� this Ϊ InInstigator, ��ͬ��EventInstigator
			EquippedWeapon->EquipWeapon(GetMesh(), FName("RightHandSocket"), this, this);
			CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon;
			OverlappingItem = nullptr;
		}
	}
	
	// ����״̬ �� ��������Ч�� Montage, ����װж����
	else if (EquippedWeapon && ActionState == EActionStates::EAS_Unoccupied && EquipMontage){
		if (CharacterState != ECharacterStates::ECS_UnEquiped) { //��ʱ����װ������״̬, ������ִ��ж����������
			ActionState = EActionStates::EAS_Equipping;
			PlayMontage(FName("Disarm"), EquipMontage);
			CharacterState = ECharacterStates::ECS_UnEquiped; // ж������������Ϊδװ��״̬
		}
		else if (CharacterState == ECharacterStates::ECS_UnEquiped) {
			ActionState = EActionStates::EAS_Equipping;
			PlayMontage(FName("Arm"), EquipMontage);
			CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon; // װ������������Ϊװ��״̬
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
	ResetAttackState(); // �����ܵ�������, ���������, �޷�ִ��
	Super::GetHited_Implementation(Impactpoint, Hitter);
}
