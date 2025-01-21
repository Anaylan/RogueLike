// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_CharacterAim.h"
#include "Actors/Characters/BaseCharacter.h"
#include "Types/EnumTypes.h"

UGameplayAbility_CharacterAim::UGameplayAbility_CharacterAim() { }

void UGameplayAbility_CharacterAim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        ABaseCharacter* Character = CastChecked<ABaseCharacter>(GetAvatarActorFromActorInfo());
        if (Character->GetGait() == EGait::Sprint) {
            Character->SetGait(EGait::Run);
        }

        PreviousRotationMode = Character->GetRotationMode();
        Character->SetRotationMode(ERotationMode::Aim);
    }
}

void UGameplayAbility_CharacterAim::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    ABaseCharacter* Character = CastChecked<ABaseCharacter>(GetAvatarActorFromActorInfo());
    Character->SetRotationMode(PreviousRotationMode);
}

void UGameplayAbility_CharacterAim::InputReleased(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL) {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
    }
}