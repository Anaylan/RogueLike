


#include "Core/Abilities/GameplayAbility_CharacterRotation.h"
#include "Actors/Characters/B_Character.h"

UGameplayAbility_CharacterRotation::UGameplayAbility_CharacterRotation()
{
}

void UGameplayAbility_CharacterRotation::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		if (!IsValid(Character) || Character->RotationMode.isOrientToMovement())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		Character->SetRotationMode(ERotationMode::OrientToMovement);
	}
}

bool UGameplayAbility_CharacterRotation::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	return IsValid(Character) && !Character->Gait.isSprint() && Super::CanActivateAbility(Handle, ActorInfo, SourceTags,
		TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_CharacterRotation::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character)) return;
	Character->SetRotationMode(ERotationMode::Strafe);
}
