// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_CharacterWalk.h"
#include "Actors/Characters/BaseCharacter.h"
#include "Types/StructTypes.h"

UGameplayAbility_CharacterWalk::UGameplayAbility_CharacterWalk()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_CharacterWalk::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}

		switch (Character->GetGait())
		{
		case EGait::Run:
			Character->SetDesiredGait(EGait::Walk);
			break;
		case EGait::Walk:
			Character->SetDesiredGait(EGait::Run);
			break;
		default:
			Character->SetDesiredGait(EGait::Run);
			break;
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

bool UGameplayAbility_CharacterWalk::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo,
                                                        const FGameplayTagContainer* SourceTags,
                                                        const FGameplayTagContainer* TargetTags,
                                                        OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && Character->GetGait() != EGait::Sprint &&
		Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
