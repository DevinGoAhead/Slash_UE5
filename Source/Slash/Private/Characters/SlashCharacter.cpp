// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Slash/Public/Items/Item.h"
#include "Slash/Public/Items/Weapons/Weapon.h"
#include "GroomComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
	: OverlappingItem(nullptr), EquippedWeapon(nullptr), 
	CharacterState(ECharacterStates::ECS_UnEquiped), ActionState(EActionStates::EAS_Unoccupied),
	AttackMontage(nullptr), EquipMontage(nullptr) {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

// ���������ص� SpineSocket
void ASlashCharacter::Disarm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabledType) {
	if (EquippedWeapon) {
		UBoxComponent* Box = EquippedWeapon->GetCollisionBox();
		if(Box) {
			Box->SetCollisionEnabled(CollisionEnabledType);
			EquippedWeapon->SetIgnoreActorsEmpty();
		}
	}
}

// ���������ص� RightHandSocket
void ASlashCharacter::Arm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::MoveForward(float Value) {
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
	// ������ Direction = (cos(Yaw + �� / 2), sin(Yaw +  �� / 2), 0.f) // Yaw ҪתΪ����
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		const FRotator ControlRotation = GetControlRotation(); // ��������ŷ���ǽṹ��
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//���� Yaw ������ת����
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Value);
	}
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

void ASlashCharacter::Attack() {
	if (CharacterState != ECharacterStates::ECS_UnEquiped 
			&& ActionState == EActionStates::EAS_Unoccupied 
			&& AttackMontage) {
		ActionState = EActionStates::EAS_Attacking;
		TArray<FName> Sections{"Attack_1", "Attack_2", "Attack_3"};
		uint8 Random = FMath::RandRange((int32)0, (int32)(Sections.Num() - 1));
		PlayMontage( Sections[Random], AttackMontage);
	}
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASlashCharacter::MoveRight);


	// ����û�ж��� ASlashCharacter::Jump, ֱ�ӵ��ø���� Jump
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("EKeyPressed"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::PlayMontage(FName MontageSction, UAnimMontage* Montage) {
	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(Montage);// ׼������״̬
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

