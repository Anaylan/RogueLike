// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_CharacterCrouch.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API UGameplayAbility_CharacterCrouch : public UGameplayAbility {
    GENERATED_BODY()

public:
    UGameplayAbility_CharacterCrouch();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
