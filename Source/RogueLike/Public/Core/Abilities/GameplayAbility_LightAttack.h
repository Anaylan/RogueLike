// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_LightAttack.generated.h"

USTRUCT()
struct FMontageAbilityData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MontageIndex = 0;
};

// struct FGameplayAbilityMontage;
/**
 *
 */
UCLASS()
class ROGUELIKE_API UGameplayAbility_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	FTimerHandle MontageTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayEffect> ExtraGameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTag QueuedOpenTag{FGameplayTag::RequestGameplayTag(FName(TEXT("Input.QueuedOpen")))};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTag HitTag{FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Hit")))};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float InputQueuedTime{.6f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	int32 MontageIndex = 0;
	// static std::atomic<int32> MontageIndex{0};

public:
	UGameplayAbility_LightAttack();

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION(BlueprintNativeEvent)
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION(Blueprintable)
	void OnPressed(float TimeWaited);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
};
