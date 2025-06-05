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
namespace EPathFollowingResult { enum Type : int; } // ö��ǰ������
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
	FVector GetRotationWarpTarget(); // ���ʷ��ص��ǵ������, �������������м���

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget(); // ͬ��

private:
	void MoveToNextPatrol();
	void MoveToTarget(AActor* Target);
	void PatrolTimerFinished();
	std::optional<double> GetTargetDistance(AActor* Traget);
	void UpdateState();
	void SetState(EEnemyStates NewState);
	void StartAttackTimer();
	void AttackTimerFinished();
	//void ResetAttackState(); // ����������ϻ�����ĳЩԭ��Ӧִ�й���������δִ�е�

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyStates EnemyState = EEnemyStates::EES_Patrolling; //Ĭ��Ѳ��״̬

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	AActor* CombatTarget = nullptr;

private:
	UPROPERTY(VisibleAnywhere, Category = "HealthItem")
	UHealthBarComponent* HealthBarWidget = nullptr; //Component, ���԰�widget �� component

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	AAIController* EnemyController = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WalkSpeed= 130.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float RunSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double CombatRadius = 1000.f; // ������, Ĭ�ϵ�λ������

	UPROPERTY(EditDefaultsOnly, Category = "AI Navigation")
	double WarpTargetDistance = 120.; // warp motion ��ɺ�, Enemy ��Ŀ��֮��ľ���

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double AttackRadius = 200.f; // �������Χ�ڼ����𹥻�

	UPROPERTY(EditAnywhere)
	float MinAttackWait = 0.35f;
	
	UPROPERTY(EditAnywhere)
	float MaxAttackWait = 0.75f;

	FTimerHandle AttackTimer;

	//TPair<int8, AActor*> PatrolTarget;, ���Ǳ���, �ʲ������
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
