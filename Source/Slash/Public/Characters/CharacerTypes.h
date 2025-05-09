#pragma once

UENUM(BlueprintType)
enum class ECharacterStates : uint8 {
	ECS_UnEquiped UMETA(Displayname = "UnEquiped"),
	ECS_EquippedOneHandedWeapon UMETA(Displayname = "Equipped One Handed Weapon"),
	EquippedTwoHandedWeapon UMETA(Displayname = "Equipped Two Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionStates : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"), // 开锁, 吃药, 攻击...都可以是空闲状态
	EAS_Attacking UMETA(DisplayName = "Attacking")
};