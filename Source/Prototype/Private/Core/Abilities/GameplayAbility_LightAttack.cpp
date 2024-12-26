// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GameplayAbility_LightAttack.h"

#include "AbilitySystemComponent.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Core/Abilities/Tasks/AbilityTask_PlayMontageAndWaitEvent.h"

void UGameplayAbility_LightAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_LightAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_LightAttack::OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Hit")))
	{
		ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (!SourceCharacter)
		{
			return;
		}

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect,
			GetAbilityLevel());

		// TODO: Implement damage from weapon
		float Damage{ 10.f };
		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::EmptyTag, Damage);
	}
	
    if (EventTag == BlockedAbilityTag)
    {
        if (CurrentActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(BlockedAbilityTag))
        {
    	    CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(BlockedAbilityTag);
        }
    }


	// AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetAvatarActorFromActorInfo());
	// if (!Hero)
	// {
	// 	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	// }
	//
	// FVector Start = Hero->GetGunComponent()->GetSocketLocation(FName("Muzzle"));
	// FVector End = Hero->GetCameraBoom()->GetComponentLocation() + Hero->GetFollowCamera()->GetForwardVector() * Range;
	// FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
	//
	// FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
	// 	
	// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
	// DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
	//
	// FTransform MuzzleTransform = Hero->GetGunComponent()->GetSocketTransform(FName("Muzzle"));
	// MuzzleTransform.SetRotation(Rotation.Quaternion());
	// MuzzleTransform.SetScale3D(FVector(1.0f));
	//
	// FActorSpawnParameters SpawnParameters;
	// SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// AGDProjectile* Projectile = GetWorld()->SpawnActorDeferred<AGDProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
	// 	Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	// Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
	// Projectile->Range = Range;
	// Projectile->FinishSpawning(MuzzleTransform);
}

void UGameplayAbility_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(BlockedAbilityTag))
    {
    	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(BlockedAbilityTag);
    }

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		const APlayableCharacter* Character = CastChecked<APlayableCharacter>(GetAvatarActorFromActorInfo());
		if (!IsValid(Character))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	
		FGameplayAbilityMontage* AbilityMontage{ MontageMapping.Find(Character->GetOverlayState()) };
		if (!AbilityMontage)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	
		UAnimMontage* MontageToPlay{ AbilityMontage->Montages[0] };
		if (!IsValid(MontageToPlay))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	
		UAbilityTask_PlayMontageAndWaitEvent* Task = UAbilityTask_PlayMontageAndWaitEvent::CreatePlayMontageAndWaitProxy(
			this, NAME_None, MontageToPlay, FGameplayTagContainer());
	
		Task->OnCancelled.AddUniqueDynamic(this, &ThisClass::OnCancelled);
		Task->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnCancelled);
		Task->OnCompleted.AddUniqueDynamic(this, &ThisClass::OnCompleted);
		Task->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
		Task->ReadyForActivation();
	}
}

bool UGameplayAbility_LightAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(BlockedAbilityTag))
		return false;

	const APlayableCharacter* Character = CastChecked<APlayableCharacter>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		IsValid(Character) && Character->CanAttack();
}

void UGameplayAbility_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(BlockedAbilityTag))
		{
			ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(BlockedAbilityTag);
		}
	}
}
