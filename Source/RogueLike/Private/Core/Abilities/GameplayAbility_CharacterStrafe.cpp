

#include "Core/Abilities/GameplayAbility_CharacterStrafe.h"
#include "Actors/Characters/BaseCharacter.h"


UGameplayAbility_CharacterStrafe::UGameplayAbility_CharacterStrafe()
{
}

void UGameplayAbility_CharacterStrafe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		switch (Character->GetRotationMode())
		{
		case ERotationMode::OrientToMovement:
			Character->SetRotationMode(ERotationMode::Strafe);
			break;
		case ERotationMode::Strafe:
			Character->SetRotationMode(ERotationMode::OrientToMovement);
			break;
		default:
			break;
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

bool UGameplayAbility_CharacterStrafe::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	const ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && Character->GetGait() != EGait::Sprint &&
		Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}