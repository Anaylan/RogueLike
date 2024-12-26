


#include "Core/Abilities/GameplayAbility_CharacterSprint.h"
#include "Actors/Characters/PlayableCharacter.h"

UGameplayAbility_CharacterSprint::UGameplayAbility_CharacterSprint()
{
}

void UGameplayAbility_CharacterSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

		APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
		if (Character->GetGait() == EGait::Sprint)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		Character->UnCrouch();
		Character->SetGait(EGait::Sprint);
	}
}

bool UGameplayAbility_CharacterSprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		IsValid(Character) && Character->CanSprint();
}

void UGameplayAbility_CharacterSprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
	Character->SetGait(EGait::Run, true);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
