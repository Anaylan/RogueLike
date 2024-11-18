// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionInterface.generated.h"
/**
 * 
 */
UINTERFACE(MinimalAPI)
class UActionInterface: public UInterface
{
	GENERATED_BODY()
};
class IActionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void EquipAction() {};

	UFUNCTION()
	virtual void AttackAction() {};
};