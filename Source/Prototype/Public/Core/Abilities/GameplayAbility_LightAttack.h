// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Types/PawnTypes.h"
#include "GameplayAbility_LightAttack.generated.h"


USTRUCT(BlueprintType)
struct FGameplayAbilityMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montages")
	TArray<TObjectPtr<class UAnimMontage>> Montages;
};
/**
 * 
 */
UCLASS(Abstract)
class PROTOTYPE_API UGameplayAbility_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	TMap<EOverlayState, FGameplayAbilityMontage> MontageMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

public:

	UGameplayAbility_LightAttack() = default;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	FGameplayTag BlockedAbilityTag{FGameplayTag::RequestGameplayTag(FName("Abilities.Blocked"))};
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
