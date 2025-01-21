// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/Attributes/PawnAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UPawnAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UPawnAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context{ Data.EffectSpec.GetContext() };
	UAbilitySystemComponent* Source{ Context.GetOriginalInstigatorAbilitySystemComponent() };
	const FGameplayTagContainer& SourceTags{ *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayAttribute& ModifiedAttribute{ Data.EvaluatedData.Attribute };

	// Compute the delta between old and new, if it is available
	[[maybe_unused]] float DeltaValue =
		(Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive) ? Data.EvaluatedData.Magnitude : 0.0f;

	APawn* TargetPawn = GetTargetPawn(Data);

	if (ModifiedAttribute == GetReceivedDamageAttribute())
	{
		AActor* SourceActor{ nullptr };
		APawn* SourcePawn{ nullptr };

		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();

			AController* SourceController{ Source->AbilityActorInfo->PlayerController.Get() };
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			SourcePawn = Cast<APawn>(SourceController->GetPawn() ? SourceController->GetPawn() : SourceActor);

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		const float LocalDamageCause = GetReceivedDamage();
		SetReceivedDamage(0.f);

		if (LocalDamageCause > 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Taken damage: %f"), LocalDamageCause)
				// Apply the health change and then clamp it
				const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageCause, 0.f, GetMaxHealth()));
		}
	}
	else if (ModifiedAttribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
}

void UPawnAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	// Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS(ThisClass, MontageIndex, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, Health, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, MaxHealth, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, Strength, Params);
}

void UPawnAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta =
			(CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

APawn* UPawnAttributeSet::GetTargetPawn(const FGameplayEffectModCallbackData& Data)
{
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		return Cast<APawn>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	}

	return nullptr;
}
