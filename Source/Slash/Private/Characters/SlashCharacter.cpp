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

	//添加毛发组件到 mesh 上
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // 绑定插槽

	Eyebrow = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrow"));
	Eyebrow->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head"); // 绑定插槽
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

// 将武器挂载到 SpineSocket
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

// 将武器挂载到 RightHandSocket
void ASlashCharacter::Arm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachToComponentSnap(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::MoveForward(float Value) {
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
	// 本质是 Direction = (cos(Yaw + π / 2), sin(Yaw +  π / 2), 0.f) // Yaw 要转为弧度
	if (ActionState == EActionStates::EAS_Unoccupied && Controller && Value) {
		const FRotator ControlRotation = GetControlRotation(); // 控制器的欧拉角结构体
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//基于 Yaw 构造旋转矩阵
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


	// 这里没有定义 ASlashCharacter::Jump, 直接调用父类的 Jump
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("EKeyPressed"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::PlayMontage(FName MontageSction, UAnimMontage* Montage) {
	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(Montage);// 准备播放状态
		AnimInstance->Montage_JumpToSection(MontageSction, Montage);
	}
}

