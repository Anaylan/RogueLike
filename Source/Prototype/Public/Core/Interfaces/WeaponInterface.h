// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.generated.h"

class ABaseWeapon;
/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponInterface: public UInterface
{
	GENERATED_BODY()
};

class IWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintable)
	virtual bool HasWeapon() = 0;

	UFUNCTION(Blueprintable)
	virtual ABaseWeapon* GetCurrentWeapon() = 0;
};