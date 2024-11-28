#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.generated.h"

/**
 * Slots abilities
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8 
{
	NONE,
	Confirm UMETA(Hidden),
	Cancel UMETA(Hidden),
	IA_Jump UMETA(DisplayName = "Jump"),
	IA_Sprint UMETA(DisplayName = "Sprint"),
	IA_Walk UMETA(DisplayName = "Walk"),
	IA_Rotation UMETA(DisplayName = "Rotation"),
	IA_Equip UMETA(DisplayName = "Equip"),
	IA_Attack UMETA(DisplayName = "Attack")
};