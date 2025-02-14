// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/GameplayAbility_LightAttack.h"
#include "AbilitySystemComponent.h"
#include "Actors/Characters/BaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Actors/Characters/EnemyCharacter.h"
#include "Core/Abilities/Tasks/AbilityTask_PlayMontageAndWaitEvent.h"
#include "Core/DataAsset/AbilityAnimSetDataAsset.h"
#include "Core/Helpers/MotionWarpingHelper.h"
#include "RogueLike/Public/Core/Helpers/AbilityHelper.h"

UGameplayAbility_LightAttack::UGameplayAbility_LightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_LightAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// MontageIndex = 0;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_LightAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGameplayAbility_LightAttack::OnPressed(float TimeWaited)
{
	if (!CurrentActorInfo || !CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Soft cancel current ability and attempt reactivation
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	CurrentActorInfo->AbilitySystemComponent->TryActivateAbility(CurrentSpecHandle, false);
}

void UGameplayAbility_LightAttack::OnEventReceived_Implementation(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (EventTag == HitTag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tag received"))
		TArray<FActiveGameplayEffectHandle> DamageEffectSpecHandle = ApplyGameplayEffectToTarget(
			CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData.TargetData,
			DamageGameplayEffect, GetAbilityLevel());

		TArray<FActiveGameplayEffectHandle> StunEffectSpecHandle = ApplyGameplayEffectToTarget(
			CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EventData.TargetData,
			ExtraGameplayEffect, GetAbilityLevel());
	}

	if (EventTag == QueuedOpenTag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Queued tag received"))
		auto InputTask =
			UAbilityTask_WaitInputPress::WaitInputPress(this);

		InputTask->OnPress.AddUniqueDynamic(this, &ThisClass::OnPressed);
		InputTask->ReadyForActivation();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		                                       [this, WeakTask = TWeakObjectPtr(InputTask)
		                                       ]()
		                                       {
			                                       if (WeakTask.IsValid())
			                                       {
				                                       WeakTask->EndTask();
			                                       }
		                                       },
		                                       InputQueuedTime, false);
	}
}

void UGameplayAbility_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());

	const auto DataAsset = Character->GetAbilitiesAnimAsset(
		FAbilityHelper::GetTagByRootTag(GetAssetTags(), FGameplayTag::RequestGameplayTag("Abilities.Type")));
	if (!IsValid(DataAsset))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityMontage& MontageData = DataAsset->FindMontage(Character->GetOverlayMode());
	if (MontageData.Montages.IsEmpty())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	int32 CurrentIndex = FPlatformAtomics::InterlockedIncrement(&MontageIndex - 1) % MontageData.Montages.Num();
	UAnimMontage* MontageToPlay = FAbilityHelper::GetAbilityMontage(MontageData, CurrentIndex);


	GetWorld()->GetTimerManager().SetTimer(MontageTimerHandle,
	                                       [this, &CurrentIndex]()
	                                       {
		                                       MontageIndex = 0;
	                                       },
	                                       MontageToPlay->GetPlayLength(), false);
	if (!IsValid(MontageToPlay))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (MontageData.bUseMotionWarping)
	{
		const FVector TargetLocation = FMotionWarpingHelper::FindNearestMotionWarpTarget(
			Character, AEnemyCharacter::StaticClass());

		FMotionWarpingHelper::ApplyMotionWarping(Character, MontageData.WarpTargetName, TargetLocation,
		                                         MontageData.WarpTargetOffset);
	}

	auto Task =
		UAbilityTask_PlayMontageAndWaitEvent::CreatePlayMontageAndWaitProxy(
			this, NAME_None, MontageToPlay, FGameplayTagContainer(), MontageData.PlayRate);

	Task->OnCancelled.AddUniqueDynamic(this, &ThisClass::OnCancelled);
	Task->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnCancelled);
	Task->OnCompleted.AddUniqueDynamic(this, &ThisClass::OnCompleted);
	Task->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
	Task->ReadyForActivation();
}
