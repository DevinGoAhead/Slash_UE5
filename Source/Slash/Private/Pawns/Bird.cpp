// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABird::ABird() {
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// �������
	Capsule = CreateDefaultSubobject <UCapsuleComponent>(TEXT("Capsule"));
	
	// ���ý�������ĳߴ�
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);

	SetRootComponent(Capsule); // ��CapsuleComponent ����Ϊ�����

	//�Ǽ�mesh���
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(Capsule); // ��BirdMesh �󶨵������-���������

	AutoPossessPlayer = EAutoReceiveInput::Player0; // �����

	// ���ɱ����
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 300.f;

	// ������
	CameraView = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraView"));
	CameraView->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ABird::BeginPlay() {
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value) {
	if (Controller && Value) {
		AddMovementInput(GetActorForwardVector(), Value); // value ������ W ��Ӱ��, ����project setting �е� scale Ӱ��
	}
}

void ABird::Turn(float Value) {
	if (Controller && Value) {
		AddControllerYawInput(Value);
		//UE_LOG(LogTemp, Warning, TEXT("%f"), Value);
	}
}

void ABird::LookUp(float Value) {
	if (Controller && Value) {
		AddControllerPitchInput(Value);
	}
}

// Called every frame
void ABird::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABird::LookUp);
}

