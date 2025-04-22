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

};
