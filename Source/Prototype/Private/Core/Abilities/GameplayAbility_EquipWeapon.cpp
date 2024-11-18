// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GameplayAbility_EquipWeapon.h"
#include "Actors/Characters/TP_Character.h"

UGameplayAbility_EquipWeapon::UGameplayAbility_EquipWeapon()
{
}

void UGameplayAbility_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		ATP_Character* Character = CastChecked<ATP_Character>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character) || Character->HasWeapon())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		//Character->Equip();
	}
}

bool UGameplayAbility_EquipWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ATP_Character* Character = CastChecked<ATP_Character>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) && IsValid(Character);
}

void UGameplayAbility_EquipWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ATP_Character* Character = CastChecked<ATP_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character)) return;

	if (Character->HasWeapon())
	{
		//Character->UnEquip();
	}
}
