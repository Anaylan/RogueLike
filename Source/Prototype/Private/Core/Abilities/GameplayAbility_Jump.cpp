// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GameplayAbility_Jump.h"
#include "Actors/Characters/B_Character.h"

UGameplayAbility_Jump::UGameplayAbility_Jump()
{
}

void UGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		Character->Jump();
		// EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character)) return;
	Character->StopJumping();
}

bool UGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && Character->CanJump() && Super::CanActivateAbility(Handle, ActorInfo, SourceTags,
		TargetTags, OptionalRelevantTags);
}