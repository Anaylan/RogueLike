#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.generated.h"

/**
 * Slots abilities
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8 
{
	None  UMETA(Hidden),
	Confirm UMETA(Hidden),
	Cancel UMETA(Hidden),
	Jump UMETA(DisplayName = "Jump"),
	Sprint UMETA(DisplayName = "Sprint"),
	Walk UMETA(DisplayName = "Walk"),
	Strafe UMETA(DisplayName = "Strafe"),
	Equip UMETA(DisplayName = "Equip"),
	Attack UMETA(DisplayName = "Attack"),
	Crouch UMETA(DisplayName = "Crouch"),
	LightAttack UMETA(DisplayNae = "LightAttack")
};

