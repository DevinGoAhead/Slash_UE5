// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UGeometryCollectionComponent;
class UCapsuleComponent;
//class FChaosBreakEvent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnIndirectlyBroken(const FChaosBreakEvent& BreakEvent);
private:
	void SpawnActor();
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
	// // // ��������Ҳ�����, ���ֲ���, ��Ȼ�����, ��Ƭ�� Detail �п�����Ƭ��pawn����ײ��Ӧȷʵ��ignore, 
	// // // ��ʵ����Ƭ��pawn Ҳȷʵ������ collision, ������Ƭ�����ɫ�ƶ���ײ��������, ��ת
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;
};
