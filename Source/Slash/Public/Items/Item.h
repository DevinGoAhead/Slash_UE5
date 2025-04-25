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

	// Called every frame
	// ��δ���ƽ�һ֡
	virtual void Tick(float DeltaTime) override;

protected:
	//UFUNCTION(BlueprintCallable) // ������Blueprint �е���
	UFUNCTION(BlueprintPure) // ����ͼ����, ����Ҫ��������
	float TransformedSin() const;

	UFUNCTION(BlueprintPure)
	float TransformedCos() const;

	// Called when the game starts or when spawned
	// ��Ϸ��ʼ������ʱ����, �麯��
	virtual void BeginPlay() override;

private:
	// VisibleInstanceOnly, ���� place �������ʵ���� detail ����ʾ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.f; // �ۼ�����ʱ��

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

protected:
	// Blueprint ͨ��������private
	// 
	// EditAnywhere, �� BP Detail(��ͼ����) �� instance detail �о��ɼ����ҿ��޸�
	// BlueprintReadWrite, ��¶�� blueprint graph, ���� get, ���� set
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeConstant = 1.f; // ���� RunningTime �ı���ϵ��

	// EditDefaultsOnly, �� BP Detail(��ͼ����) �ɼ�, instance detail �пɼ����ҿ��޸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amplitude = 1.f; // ���, ��� sin cos ����
};
