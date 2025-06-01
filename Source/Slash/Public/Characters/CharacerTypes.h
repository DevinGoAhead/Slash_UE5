#pragma once

UENUM(BlueprintType)
enum class ECharacterStates : uint8 {
	ECS_UnEquiped UMETA(Displayname = "UnEquiped"),
	ECS_EquippedOneHandedWeapon UMETA(Displayname = "Equipped One Handed Weapon"),
	EquippedTwoHandedWeapon UMETA(Displayname = "Equipped Two Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionStates : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"), // ����, ��ҩ, ����...�����״̬
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
	EES_None UMETA(DisplayName = "None"), // ������ҪΪ�˸����� Engaged �� Attacking ֮���л�
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Alerting UMETA(DisplayName = "Alerting"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"), // �����Ǵ��ڹ���״̬, ����һ����ִ�й�������, �����ǹ���������Ъ�ڼ�
	EES_Engaged UMETA(DisplayName = "Engaged") // ���ڹ���, ���������õ���, ���ڷ���
};