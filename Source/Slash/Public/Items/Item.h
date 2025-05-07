// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" // ���� AActor ���ڵ�ͷ�ļ�
#include "Item.generated.h" // C++ ���� UE �������

class USphereComponent;
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

	UFUNCTION()
	virtual void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// Called when the game starts or when spawned
	// ��Ϸ��ʼ������ʱ����, �麯��
	virtual void BeginPlay() override;

protected:
	// Blueprint ͨ��������private
	// 
	// EditAnywhere, �� BP Detail(��ͼ����) �� instance detail �о��ɼ����ҿ��޸�
	// BlueprintReadWrite, ��¶�� blueprint graph, ���� get, ���� set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move And Rotate")
	float TimeConstant; // ���� RunningTime �ı���ϵ��

	// EditDefaultsOnly, �� BP Detail(��ͼ����) �ɼ�, instance detail �пɼ����ҿ��޸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move And Rotate")
	float Amplitude; // ���, ��� sin cos ����

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
private:
	// VisibleInstanceOnly, ���� place �������ʵ���� detail ����ʾ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.f; // �ۼ�����ʱ��

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
};
