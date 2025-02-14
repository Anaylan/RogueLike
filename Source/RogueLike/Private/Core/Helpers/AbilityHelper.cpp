// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Helpers/AbilityHelper.h"
#include "Core/DataAsset/AbilityAnimSetDataAsset.h"

FGameplayTag FAbilityHelper::GetTagByRootTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& RootTag)
{
	for (const FGameplayTag& Tag : AbilityTags)
	{
		if (Tag.MatchesTag(RootTag))
		{
			return Tag;
		}
	}
	return FGameplayTag::EmptyTag;
}

UAnimMontage* FAbilityHelper::GetAbilityMontage(const FGameplayAbilityMontage& Data, int32& Index)
{
	if (Data.Montages.IsEmpty())
	{
		return nullptr;
	}

	if (Data.bRandomSelect)
	{
		return Data.Montages[FMath::RandRange(0, Data.Montages.Num() - 1)];
	}

	// static TMap<const FGameplayAbilityMontage*, int32> SelectionIndices;
	// Index = SelectionIndices.FindOrAdd(&Data);
	return Data.Montages[Index];
}
