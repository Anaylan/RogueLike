// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_CharacterCrouch.h"
#include "Actors/Characters/BaseCharacter.h"

void UGameplayAbility_CharacterCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		ABaseCharacter* Character = CastChecked<ABaseCharacter>(GetAvatarActorFromActorInfo());
		if (Character->GetGait() == EGait::Sprint)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}

		switch (Character->GetStanceMode())
		{
		case EStanceMode::Crouch:
			Character->UnCrouch();
			break;
		default:
			Character->Crouch();
			break;
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
