#pragma once

UENUM(BlueprintType)
enum class ECharacterStates : uint8 {
	ECS_UnEquiped UMETA(Displayname = "UnEquiped"),
	ECS_EquippedOneHandedWeapon UMETA(Displayname = "Equipped One Handed Weapon"),
	EquippedTwoHandedWeapon UMETA(Displayname = "Equipped Two Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionStates : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"), // 开锁, 吃药, 攻击...以外的状态
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8 {
//enum EDeathPose {
	EDP_NoState UMETA(DisplayName = "NoState"),
	EDP_Death1  UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),
	EDP_Death7 UMETA(DisplayName = "Death7"),
	EDP_Death8 UMETA(DisplayName = "Death8"),
	EDP_Death9 UMETA(DisplayName = "Death9"),
	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyStates : uint8 {
	EES_None UMETA(DisplayName = "None"), // 这里主要为了辅助在 Engaged 和 Attacking 之间切换
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Alerting UMETA(DisplayName = "Alerting"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"), // 这里是处于攻击状态, 但不一定在执行攻击动作, 可能是攻击动作间歇期间
	EES_Engaged UMETA(DisplayName = "Engaged") // 正在攻击, 比如正在用刀砍, 正在发射
};