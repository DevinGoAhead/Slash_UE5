// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" // 这是 AActor 所在的头文件
#include "Item.generated.h" // C++ 参与 UE 反射相关

class USphereComponent;
UCLASS()
class SLASH_API AItem : public AActor {
	GENERATED_BODY() // C++ 参与 UE 反射相关	
public:	
	// Sets default values for this actor's properties
	// 设置 Actor 的默认属性
	AItem();

	// Called every frame
	// 向未来推进一帧
	virtual void Tick(float DeltaTime) override;

protected:
	//UFUNCTION(BlueprintCallable) // 允许在Blueprint 中调用
	UFUNCTION(BlueprintPure) // 纯蓝图函数, 不需要输入和输出
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
	// 游戏开始或被生成时调用, 虚函数
	virtual void BeginPlay() override;

protected:
	// Blueprint 通常不能是private
	// 
	// EditAnywhere, 在 BP Detail(蓝图窗口) 和 instance detail 中均可见并且可修改
	// BlueprintReadWrite, 暴露给 blueprint graph, 可以 get, 可以 set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move And Rotate")
	float TimeConstant; // 用于 RunningTime 的倍增系数

	// EditDefaultsOnly, 在 BP Detail(蓝图窗口) 可见, instance detail 中可见并且可修改
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move And Rotate")
	float Amplitude; // 振幅, 配合 sin cos 函数

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
private:
	// VisibleInstanceOnly, 仅在 place 到世界的实例的 detail 中显示
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.f; // 累计运行时间

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
};
