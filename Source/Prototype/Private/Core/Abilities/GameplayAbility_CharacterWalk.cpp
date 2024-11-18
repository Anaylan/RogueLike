// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GameplayAbility_CharacterWalk.h"
#include "Actors/Characters/B_Character.h"
#include "Core/Types/StructTypes.h"

UGameplayAbility_CharacterWalk::UGameplayAbility_CharacterWalk()
{
}

void UGameplayAbility_CharacterWalk::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());

		if (!IsValid(Character) || Character->Gait.isWalk()) 
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		Character->SetGait(EGait::Walk);
	}
}

bool UGameplayAbility_CharacterWalk::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && !Character->Gait.isSprint() 
		&& Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_CharacterWalk::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());

	if (!IsValid(Character)) return;
	Character->SetGait(EGait::Run);	
}
