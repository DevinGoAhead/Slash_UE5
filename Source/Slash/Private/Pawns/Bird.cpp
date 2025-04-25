// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABird::ABird() {
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 胶囊组件
	Capsule = CreateDefaultSubobject <UCapsuleComponent>(TEXT("Capsule"));
	
	// 设置胶囊组件的尺寸
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);

	SetRootComponent(Capsule); // 将CapsuleComponent 设置为根组件

	//骨架mesh组件
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(Capsule); // 将BirdMesh 绑定到根组件-胶囊组件上

	AutoPossessPlayer = EAutoReceiveInput::Player0; // 绑定玩家

	// 弹簧臂组件
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 300.f;

	// 相机组件
	CameraView = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraView"));
	CameraView->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ABird::BeginPlay() {
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value) {
	if (Controller && Value) {
		AddMovementInput(GetActorForwardVector(), Value); // value 除了受 W 键影响, 还受project setting 中的 scale 影响
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

