// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

// Sets default values
AItem::AItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 如果 actor 不需要参与 tick, 例如 静态对象, 可以设置为 false
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	// Super::, 调用父类成员函	数
	// 可能父类完成了一些功能
	Super::BeginPlay();

	// 输出到 UE output Log
	// UE_LOG() 宏
	// 日志类型, 详细等级
	// TEXT(), 宏, 字符串字面量转换为 Unicode
	UE_LOG(LogTemp, Warning, TEXT("AItem::BeginPlay() called!"));

	// 输出到UE窗口
	if (GEngine) {
		// key, 与蓝图中key 含义相同, 同样的key值标记的输出内容会被覆盖
		// TimeToDisplay, DeBugMessage 从显示到消失的时间, 相同key 的消息被覆盖后, TimeToDisplay 重置
		// bNewerOnTop, 新的消息会显示在已有消息的上方, 只有当 key = INDEX_NONE 时, bNewerOnTop 才会生效
		GEngine->AddOnScreenDebugMessage(3, 25.f, FColor::Blue, FString("AItem::Begin() output in screnn!"));
	}
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// 获取 class Item 实例化对象的名称
	// 比如将一个 class 对象 place 到游戏中, 然后 alt 复制一份, 二者是不同的名字, 他们会调用各自的 AItem::Tick
	// 进而在下面的调试输出函数中输出不同的名字
	FString ItemName = GetName();

	// Printf 可将内容格式化, 类似 sprintf
	// *name, 返回 c 字符串, FString 重载了*, UE 中格式化 FString 必须使用*
	FString OutPut = FString::Printf(TEXT("Item Name:, % s, Tick Time: % f"), *ItemName, DeltaTime);


	UE_LOG(LogTemp, Warning, TEXT("%s"), *OutPut);
	UE_LOG(LogTemp, Warning, TEXT("%2f"), DeltaTime); // 一个额外测试, 测试  TEXT
	GEngine->AddOnScreenDebugMessage(1, 25.f, FColor::Magenta, OutPut);
}

