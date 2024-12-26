


#include "Core/Abilities/GameplayAbility_CharacterStrafe.h"
#include "Actors/Characters/PlayableCharacter.h"

UGameplayAbility_CharacterStrafe::UGameplayAbility_CharacterStrafe()
{
}

void UGameplayAbility_CharacterStrafe::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
		if (!IsValid(Character) || Character->GetRotationMode() == ERotationMode::OrientToMovement)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		Character->SetRotationMode(ERotationMode::OrientToMovement);
	}
}

bool UGameplayAbility_CharacterStrafe::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && Character->GetGait() != EGait::Sprint && Super::CanActivateAbility(Handle, ActorInfo, SourceTags,
		TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_CharacterStrafe::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character)) return;
	Character->SetRotationMode(ERotationMode::Strafe);
}
