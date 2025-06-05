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

#include <optional>

#include "Characters/BaseCharacter.h"
#include "TimerManager.h" 
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Destroyed();
	
	virtual void GetHited_Implementation(const FVector& Impactpoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
							AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual void Attack();
	virtual void AttackEnd() override;

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	UFUNCTION()
	void OnPawnSee(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget(); // 本质返回的是点的坐标, 引擎根据坐标进行计算

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget(); // 同上

private:
	void MoveToNextPatrol();
	void MoveToTarget(AActor* Target);
	void PatrolTimerFinished();
	std::optional<double> GetTargetDistance(AActor* Traget);
	void UpdateState();
	void SetState(EEnemyStates NewState);
	void StartAttackTimer();
	void AttackTimerFinished();
	//void ResetAttackState(); // 当攻击被打断或由于某些原因本应执行攻击动作但未执行的

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyStates EnemyState = EEnemyStates::EES_Patrolling; //默认巡逻状态

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	AActor* CombatTarget = nullptr;

private:
	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UHealthBarComponent* HealthBarWidget = nullptr; //Component, 可以绑定widget 的 component

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	AAIController* EnemyController = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WalkSpeed= 130.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float RunSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double CombatRadius = 1000.f; // 经测试, 默认单位是厘米

	UPROPERTY(EditDefaultsOnly, Category = "AI Navigation")
	double WarpTargetDistance = 120.; // warp motion 完成后, Enemy 与目标之间的距离

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double AttackRadius = 200.f; // 在这个范围内即发起攻击

	UPROPERTY(EditAnywhere)
	float MinAttackWait = 0.35f;
	
	UPROPERTY(EditAnywhere)
	float MaxAttackWait = 0.75f;

	FTimerHandle AttackTimer;

	//TPair<int8, AActor*> PatrolTarget;, 总是报错, 故拆解如下
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget = nullptr; // current PatrolTarget
	
	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	int8 PatrolTargetIndex = -1; 

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float MinPatrolWait = 2.f;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float MaxPatrolWait = 6.f;
	FTimerHandle PatrolTimer;

	UPROPERTY(VisibleAnywhere, Category = "AI Navigation")
	UPawnSensingComponent* PawnSensing;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASoul> SoulClass;
};
