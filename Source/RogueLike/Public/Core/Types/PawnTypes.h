#pragma once

#include "CoreMinimal.h"
#include "PawnTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	OneHanded,
	TwoHanded
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	// this field need to
	None,
	Left,
	Right
};