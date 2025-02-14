// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

struct FGameplayAbilityMontage;
/**
 * 
 */
class ROGUELIKE_API FAbilityHelper
{
public:
	FAbilityHelper() = default;

	static FGameplayTag GetTagByRootTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& RootTag);

	static UAnimMontage* GetAbilityMontage(const FGameplayAbilityMontage& Data, int32& Index);
};
