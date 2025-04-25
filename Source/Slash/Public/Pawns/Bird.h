// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// 输入控制组件
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
private:
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);
private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule; // 仅仅是指针 visible, 指针指向的内容是 Editable

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraView;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
};
