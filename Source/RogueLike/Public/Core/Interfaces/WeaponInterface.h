// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.generated.h"

class ABaseWeapon;
/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class ROGUELIKE_API IWeaponInterface
{
	GENERATED_BODY()

public:
	/** Check if entity has any weapons */
	UFUNCTION(Blueprintable, Category = "Weapon Interface")
	virtual bool HasWeapons() = 0;

	UFUNCTION(Blueprintable, Category = "Weapon Interface")
	virtual ABaseWeapon* GetWeaponInSlot(const struct FGameplayTag Slot) = 0;

	UFUNCTION(Blueprintable, Category = "Weapon Interface")
	virtual bool SwitchWeapon(const FGameplayTag& WeaponSlot, int32 Index) const = 0;
	UFUNCTION(Blueprintable, Category = "Weapon Interface")
	virtual int32 GetEquippedWeaponsSize() const = 0;
};
