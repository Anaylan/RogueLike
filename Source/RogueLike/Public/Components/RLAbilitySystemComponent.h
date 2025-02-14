// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RLAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup=AbilitySystem, meta=(BlueprintSpawnableComponent))
class ROGUELIKE_API URLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	URLAbilitySystemComponent();
};
