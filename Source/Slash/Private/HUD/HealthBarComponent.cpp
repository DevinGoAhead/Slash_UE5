// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "Slash/Public//HUD/HealthBar.h"

#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent) {
	// ��һ���Ǵ����
	// ��Ϊ�� void AEnemy::BeginPlay()  �г�ʼ��ʱ������һ�� SetHealthPercent, ���� HealthBarWidget != nullptr
	//if (HealthBarWidget == nullptr) {
	
	HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	if (HealthBarWidget && HealthBarWidget->HealthBar) {
		HealthBarWidget->HealthBar->SetPercent(Percent); // �������� ProgressBar �� detail �е�����
	}
}
