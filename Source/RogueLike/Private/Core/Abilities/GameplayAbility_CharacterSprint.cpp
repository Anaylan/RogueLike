#include "Core/Abilities/GameplayAbility_CharacterSprint.h"
#include "Actors/Characters/BaseCharacter.h"

UGameplayAbility_CharacterSprint::UGameplayAbility_CharacterSprint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_CharacterSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		if (Character->GetGait() == EGait::Sprint)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		Character->UnCrouch();
		Character->SetDesiredGait(EGait::Sprint);
	}
}

bool UGameplayAbility_CharacterSprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo,
                                                          const FGameplayTagContainer* SourceTags,
                                                          const FGameplayTagContainer* TargetTags,
                                                          FGameplayTagContainer* OptionalRelevantTags) const
{
	ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		IsValid(Character) && Character->CanSprint();
}

void UGameplayAbility_CharacterSprint::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != nullptr && ActorInfo->AvatarActor.IsValid())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGameplayAbility_CharacterSprint::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                                     bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	Character->SetDesiredGait(EGait::Run, true);
}
