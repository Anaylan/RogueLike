


#include "Core/Abilities/GameplayAbility_Sprint.h"
#include "Actors/Characters/B_Character.h"

UGameplayAbility_Sprint::UGameplayAbility_Sprint()
{
}

void UGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		if (Character->Gait.isSprint())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		// Character->UnCrouch();
		if (Character->RotationMode.isStrafe()) Character->SetRotationMode(ERotationMode::OrientToMovement);
		Character->SetGait(EGait::Sprint);
	}
}

void UGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AB_Character* Character = CastChecked<AB_Character>(ActorInfo->AvatarActor.Get());
	Character->SetGait(EGait::Run, true);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
