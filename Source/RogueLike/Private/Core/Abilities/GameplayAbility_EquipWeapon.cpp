// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_EquipWeapon.h"
#include "Actors/Characters/BaseCharacter.h"

UGameplayAbility_EquipWeapon::UGameplayAbility_EquipWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		/*ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character) || Character->HasWeapons())
		{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
		}
		
		Character->Equip();*/
	}
}

bool UGameplayAbility_EquipWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayTagContainer* SourceTags,
                                                      const FGameplayTagContainer* TargetTags,
                                                      OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		IsValid(Character);
}

void UGameplayAbility_EquipWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	/*ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
	{
		return;
	}*/
}
