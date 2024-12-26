// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/Attributes/PawnAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UPawnAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthPointsAttribute())
	{
		AdjustAttributeForMaxChange(HealthPoints, MaxHealthPoints, NewValue,
			GetHealthPointsAttribute());
	}
}

void UPawnAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	ACharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// AController* TargetController = nullptr;
		AActor* TargetActor = nullptr;
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		// TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetCausedDamageAttribute())
	{
		AActor* SourceActor = nullptr;
		ACharacter* SourceCharacter = nullptr;

		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			AController* SourceController = nullptr;
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ACharacter>(SourceActor);
			}

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}
		
		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		const float LocalDamageCause = GetCausedDamage();
		SetCausedDamage(0.f);

		if (LocalDamageCause > 0.f)
		{
			// Apply the health change and then clamp it
			const float OldHealthPoints = GetHealthPoints();
			SetHealthPoints(FMath::Clamp(OldHealthPoints - LocalDamageCause, 0.f, GetMaxHealthPoints()));

			if (TargetCharacter)
			{
				// This is proper damage
				// TargetCharacter->HandleDamage(LocalDamageCause, HitResult, SourceTags, SourceCharacter, SourceActor);

				// Call for all health changes
				// TargetCharacter->HandleHealthChanged(-LocalDamageCause, SourceTags);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthPointsAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealthPoints(FMath::Clamp(GetHealthPoints(), 0.0f, GetMaxHealthPoints()));

		if (TargetCharacter)
		{
			// Call for all health changes
			// TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
}

void UPawnAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ThisClass, MontageIndex, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, PlayRate, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, HealthPoints, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, MaxHealthPoints, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, CausedDamage, Params);
}

void UPawnAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f)
								   ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
								   : NewMaxValue;
		
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UPawnAttributeSet::OnRep_PlayRate(const FGameplayAttributeData& OldPlayRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PlayRate, OldPlayRate);
}

void UPawnAttributeSet::OnRep_MontageIndex(const FGameplayAttributeData& OldMontageIndex)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MontageIndex, OldMontageIndex);
}

void UPawnAttributeSet::OnRep_HealthPoints(const FGameplayAttributeData& OldHealthPoints)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HealthPoints, OldHealthPoints);
}

void UPawnAttributeSet::OnRep_MaxHealthPoints(const FGameplayAttributeData& OldMaxHealthPoints)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealthPoints, OldMaxHealthPoints);
}
