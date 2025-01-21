// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Types/AbilityTypes.h"
#include "Types/EnumTypes.h"
#include "GameplayAbility_LightAttack.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API UGameplayAbility_LightAttack : public UGameplayAbility {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TMap<EOverlayState, FGameplayAbilityMontage> MontageMapping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TSubclassOf<UGameplayEffect> DamageGameplayEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MuzzleSocket{FName(TEXT("Muzzle"))};

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FGameplayTag BlockedAbilityTag { FGameplayTag::RequestGameplayTag(FName(TEXT("Abilities.Blocked"))) };

public:
    UGameplayAbility_LightAttack() = default;

    UFUNCTION(Blueprintable)
    void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
    UFUNCTION(Blueprintable)
    void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
    void OnEventReceived_Implementation(FGameplayTag EventTag, FGameplayEventData EventData);
    UFUNCTION(BlueprintNativeEvent)
    void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr,
        FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
        bool bWasCancelled) override;
};
