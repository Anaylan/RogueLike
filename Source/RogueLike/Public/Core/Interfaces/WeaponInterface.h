// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.generated.h"

class ABaseWeapon;
enum class EWeaponSlot : uint8;
/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponInterface : public UInterface {
    GENERATED_BODY()
};

class ROGUELIKE_API IWeaponInterface {
    GENERATED_BODY()

public:
    UFUNCTION(Blueprintable)
    virtual bool HasWeapon(EWeaponSlot WeaponSlot) = 0;
    UFUNCTION(Blueprintable)
    virtual bool CanAttack() const = 0;
    UFUNCTION(Blueprintable)
    virtual bool CanEquipWeapon() const = 0;

    UFUNCTION(Blueprintable)
    virtual ABaseWeapon* GetCurrentWeapon(EWeaponSlot WeaponHand) = 0;
};