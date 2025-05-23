// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


class USoundBase;
class UBoxComponent;
class APawn;

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
GENERATED_BODY() // C++ 参与 UE 反射相关, 
public:
	AWeapon();
	void EquipWeapon(USceneComponent* Inparent, const FName& InSocket, AActor* NewOwner, APawn* InInstigator);
	void AttachToComponentSnap(USceneComponent* Inparent, const FName& InSocket);
	FORCEINLINE UBoxComponent* GetCollisionBox() { return CollisionBox; }
	void SetIgnoreActorsEmpty();
protected:
	virtual void OnSphereBeginOverlap (
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;


	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	// C++ 中仅声明, 且在C++ 中可调用, 蓝图中完成定义, 不能是 private
	// 在蓝图中可作为一个事件
	UFUNCTION(BlueprintImplementableEvent)
	void CreateField(const FVector& Location);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* CollisionBox;

	USceneComponent* TraceStart;
	USceneComponent* TraceEnd;
	TArray<AActor*> IgnoreActors;
	float BaseDamage = 20.f;
};
