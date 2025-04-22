// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" // ���� AActor ���ڵ�ͷ�ļ�
#include "Item.generated.h" // C++ ���� UE �������

UCLASS()
class SLASH_API AItem : public AActor {
	GENERATED_BODY() // C++ ���� UE �������
	
public:	
	// Sets default values for this actor's properties
	// ���� Actor ��Ĭ������
	AItem();

protected:
	// Called when the game starts or when spawned
	// ��Ϸ��ʼ������ʱ����, �麯��
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// ��δ���ƽ�һ֡
	virtual void Tick(float DeltaTime) override;

};
