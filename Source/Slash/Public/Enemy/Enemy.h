// ill out your copyright notice in the Description page of Project Settings.

#pragma once

class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UAttributeComponent;
class UWidgetComponent;
class UHealthBarComponent;
class AAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; } // 枚举前向声明
class UPawnSensingComponent;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Public/Interfaces/HitInterface.h"
#include "Slash/Public/Characters/CharacerTypes.h"
#include "TimerManager.h" 
#include <optional>
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void GetHited_Implementation(const FVector& Impactpoint) override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
							AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	UFUNCTION()
	void OnPawnSee(APawn* Pawn);
private:
	void PlayMontage(FName MontageSction, UAnimMontage* Montage);
	FString GetHitedDirection(const FVector& Impactpoint);
	void Die();
	void MoveToNextPatrol();
	void PatrolTimerFinished();
	std::optional<double> GetTargetDistance(AActor* Traget);
	void MoveToTarget(AActor* Target);
	void UpdateState();
	void SetState(EEnemyStates NewState);
private:
	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* HitedMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase* HitedSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitedParticle = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UAttributeComponent* Attributes = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UHealthBarComponent* HealthBarWidget = nullptr; //Component, 可以绑定widget 的 component

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 1000.f; // 经测试, 默认单位是厘米

	UPROPERTY(EditAnywhere, Category = "Combat")
	AActor* CombatTarget = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double AttackRadius = 180.f; // 在这个范围内即发起攻击

	//TPair<int8, AActor*> PatrolTarget; // current, 总是报错, 故拆解如下
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	int8 PatrolTargetIndex = -1;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY()
	AAIController* EnemyController = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float MinWaitTime = 2.f;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float MaxWaitTime = 6.f;
	FTimerHandle PatrolTimer;

	UPROPERTY(VisibleAnywhere, Category = "AI Navigation")
	UPawnSensingComponent* PawnSensing;

	EEnemyStates EnemyState = EEnemyStates::EES_Patrolling; //默认巡逻状态
};
