// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
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
void ASlashCharacter::MoveForward(float Value) {
	// ����� forward ���� �����ſ�������ת, �����ת���ܻᵼ�� forward ����������б, ������ֻϣ����� XZ ƽ��ķ���
	// ����ĵ���ת�� Controller ��һ�µ�, ��������Ҫ��forward XZ ƽ��ķ������ʾ��� Controller ��ת�������� Yaw Ӱ��Ĳ���
	// �������ΪXZ ƽ���һ����λ����, �ۼ�ת�� Yaw ֮��ķ���, �� (cos(Yaw), sin(Yaw), 0.f)
	// Direction = (cos(Yaw), sin(Yaw), 0.f) // Yaw ҪתΪ����
	// ʾ��������ʹ����һ�ָ����׵ķ�ʽ����
	if (Controller && Value) {
		const FRotator ControlRotation = GetControlRotation(); // ��������ŷ���ǽṹ��
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);//���� Yaw ������ת����
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Value);
	}
}

void ASlashCharacter::MoveRight(float Value) {
	// ������ Direction = (cos(Yaw + �� / 2), sin(Yaw +  �� / 2), 0.f) // Yaw ҪתΪ����
	if (Controller && Value) {
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
}

