// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" // 这是 AActor 所在的头文件
#include "Item.generated.h" // C++ 参与 UE 反射相关

UCLASS()
class SLASH_API AItem : public AActor {
	GENERATED_BODY() // C++ 参与 UE 反射相关
	
public:	
	// Sets default values for this actor's properties
	// 设置 Actor 的默认属性
	AItem();

protected:
	// Called when the game starts or when spawned
	// 游戏开始或被生成时调用, 虚函数
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// 向未来推进一帧
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleInstanceOnly); // 仅在 place 到世界的实例的 detail 中显示
	float RunningTime = 0.f; // 累计运行时间

	UPROPERTY(EditAnywhere); // 在 BP Detail(蓝图窗口) 和 instance detail 中均可见并且可修改
	float Amplitude = 1.f; // 振幅, 配合 sin cos 函数

	UPROPERTY(EditAnywhere);
	float TimeConstant = 1.f; // 用于 RunningTime 的倍增系数
};
