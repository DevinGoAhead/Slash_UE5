// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "Slash/Public//HUD/HealthBar.h"

#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent) {
	// 这一步是错误的
	// 因为在 void AEnemy::BeginPlay()  中初始化时调用了一次 SetHealthPercent, 导致 HealthBarWidget != nullptr
	//if (HealthBarWidget == nullptr) {
	
	HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	if (HealthBarWidget && HealthBarWidget->HealthBar) {
		HealthBarWidget->HealthBar->SetPercent(Percent); // 这是设置 ProgressBar 的 detail 中的属性
	}
}
