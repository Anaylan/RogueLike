// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_CharacterAim.generated.h"

enum class ERotationMode : uint8;
/**
 *
 */
UCLASS()
class ROGUELIKE_API UGameplayAbility_CharacterAim : public UGameplayAbility {
    GENERATED_BODY()

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    ERotationMode PreviousRotationMode;

public:
    UGameplayAbility_CharacterAim();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo);

    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
};
