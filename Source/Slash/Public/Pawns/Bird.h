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
	// ����������
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
private:
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);
private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule; // ������ָ�� visible, ָ��ָ��������� Editable

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraView;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
};
