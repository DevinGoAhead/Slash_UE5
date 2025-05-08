#pragma once

UENUM(BlueprintType)
enum class ECharacterStates : uint8 {
	ECS_UnEquiped UMETA(Displayname = "UnEquiped"),
	ECS_EquippedOneHandedWeapon UMETA(Displayname = "Equipped One Handed Weapon"),
	EquippedTwoHandedWeapon UMETA(Displayname = "Equipped Two Handed Weapon")
};
