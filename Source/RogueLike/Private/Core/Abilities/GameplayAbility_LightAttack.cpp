// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_LightAttack.h"
#include "AbilitySystemComponent.h"
#include "Actors/Characters/BaseCharacter.h"
#include "Actors/Weapons/BaseWeapon.h"
#include "Actors/Projectiles/BaseProjectile.h"
#include "Core/Abilities/Tasks/AbilityTask_PlayMontageAndWaitEvent.h"

void UGameplayAbility_LightAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_LightAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_LightAttack::OnEventReceived_Implementation(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Hit")))
	// {
	// 	ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData.TargetData,
	// 		DamageGameplayEffect, 1);
	// }

	if (EventTag == BlockedAbilityTag)
	{
		SetCanBeCanceled(true);
	}

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
		if (!Character)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		USkeletalMeshComponent* WeaponMesh = Character->GetCurrentWeapon()->GetMesh();
		FVector Start = WeaponMesh->GetSocketLocation(MuzzleSocket);
		FVector End = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f;
		FRotator Rotation = FRotationMatrix::MakeFromX(Start - End).Rotator();

		FGameplayEffectSpecHandle DamageEffectSpecHandle{ MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel())};

		FTransform MuzzleTransform = WeaponMesh->GetSocketTransform(MuzzleSocket);
		MuzzleTransform.SetRotation(Rotation.Quaternion());
		MuzzleTransform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
			Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
		Projectile->Range = 10.f;
		Projectile->FinishSpawning(MuzzleTransform);
	}
}

void UGameplayAbility_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		const ABaseCharacter* Character = CastChecked<ABaseCharacter>(GetAvatarActorFromActorInfo());
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

		UAbilityTask_PlayMontageAndWaitEvent* Task =
			UAbilityTask_PlayMontageAndWaitEvent::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay,
				FGameplayTagContainer());

		Task->OnCancelled.AddUniqueDynamic(this, &ThisClass::OnCancelled);
		Task->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnCancelled);
		Task->OnCompleted.AddUniqueDynamic(this, &ThisClass::OnCompleted);
		Task->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
		Task->ReadyForActivation();
	}
}

bool UGameplayAbility_LightAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	const ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		IsValid(Character);
}

void UGameplayAbility_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
