// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Types/GameplayTags.h"

namespace WeaponSlotTags
{
	UE_DEFINE_GAMEPLAY_TAG(None, FName{TEXTVIEW("Weapon.Slot.None")})
	UE_DEFINE_GAMEPLAY_TAG(Secondary, FName{TEXTVIEW("Weapon.Slot.Secondary")})
	UE_DEFINE_GAMEPLAY_TAG(Primary, FName{TEXTVIEW("Weapon.Slot.Primary")})
}
