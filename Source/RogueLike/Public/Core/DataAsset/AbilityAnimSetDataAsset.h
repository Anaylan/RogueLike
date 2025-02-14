// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Types/EnumTypes.h"
#include "AbilityAnimSetDataAsset.generated.h"

USTRUCT(BlueprintType)
struct ROGUELIKE_API FGameplayAbilityMontage
{
	GENERATED_BODY()

	// For filtering montages by category
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", Meta = (Categories = "Ability"))
	FGameplayTagContainer AdditionalTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	TArray<TObjectPtr<UAnimMontage>> Montages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	float PlayRate{1.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	uint8 bLoop : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	uint8 bRandomSelect : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	uint8 bUseMotionWarping : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Warping",
		meta = (EditCondition = "bUseMotionWarping"))
	FName WarpTargetName{NAME_None};

	// Смещение относительно цели
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Warping",
		meta = (EditCondition = "bUseMotionWarping"))
	FVector WarpTargetOffset{FVector::ZeroVector};
};


/**
 * 
 */
UCLASS()
class ROGUELIKE_API UAbilityAnimSetDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	static const FPrimaryAssetType AssetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	TMap<FGameplayTag, FGameplayAbilityMontage> Animations;

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Animations")
	const FGameplayAbilityMontage& FindMontage(const FGameplayTag OverlayMode,
	                                           const FGameplayTagContainer& ContextTags = FGameplayTagContainer())
	const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}

protected: // Fallback данные
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	FGameplayAbilityMontage FallbackData;
};
