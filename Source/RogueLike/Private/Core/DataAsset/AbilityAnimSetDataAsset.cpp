// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DataAsset/AbilityAnimSetDataAsset.h"

const FPrimaryAssetType UAbilityAnimSetDataAsset::AssetType = TEXT("AbilityAnimSet");

const FGameplayAbilityMontage& UAbilityAnimSetDataAsset::FindMontage(const FGameplayTag OverlayMode,
                                                                     const FGameplayTagContainer& ContextTags) const
{
	const FGameplayAbilityMontage* BestMatch = Animations.Find(OverlayMode);

	if (!BestMatch)
	{
		return FallbackData;
	}

	if (ContextTags.IsEmpty() || ContextTags.HasAll(BestMatch->AdditionalTags))
	{
		return *BestMatch;
	}

	return FallbackData;
}
