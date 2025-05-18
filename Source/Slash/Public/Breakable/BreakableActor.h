// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UGeometryCollectionComponent;
class UCapsuleComponent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere);
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "PickupClasses")
	/* UClass* TreasureClass //������ʽ�ᵼ����ͼ Detail �б��пɹ�ѡ��Ľ������м̳���UClass ���� */
	TArray<TSubclassOf<class ATreasure>> TreasureClasses; // �������޶�Ϊ ATreasure ��������

	// ����: GetHited ����ε���, ��������ѭ��, ��ջ���
	bool bBroken;

	// ����, ��������, ��Ƭ�� block ��ҽ�ɫ
	// ֱ��GeometryCollection ����pawn, Ȼ�����һ������, ���� Pawn ��ײ��Ӧ block, ������ٺ���Pawn, ʾ��ʹ�õ��������
	// ����Ϊʲô��ֱ�Ӵ����ֱ�� GeometryCollection ����pawn?
	// ��׼������һ��
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;
};
