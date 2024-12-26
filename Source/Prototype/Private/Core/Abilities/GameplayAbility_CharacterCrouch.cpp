// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GameplayAbility_CharacterCrouch.h"
#include "Actors/Characters/PlayableCharacter.h"

void UGameplayAbility_CharacterCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		APlayableCharacter* Character = CastChecked<APlayableCharacter>(GetAvatarActorFromActorInfo());
		if (Character->GetGait() == EGait::Sprint || Character->GetStanceMode() == EStanceMode::Crouch)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		Character->Crouch();
	}
}

void UGameplayAbility_CharacterCrouch::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	APlayableCharacter* Character = CastChecked<APlayableCharacter>(GetAvatarActorFromActorInfo());
	Character->UnCrouch();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
