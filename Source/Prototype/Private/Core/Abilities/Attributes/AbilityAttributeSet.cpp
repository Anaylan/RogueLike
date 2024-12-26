// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/Attributes/AbilityAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UAbilityAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAbilityAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAbilityAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ThisClass, MontageIndex, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, PlayRate, Params);
}

void UAbilityAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
}

void UAbilityAttributeSet::OnRep_PlayRate(const FGameplayAttributeData& OldPlayRate)
{
}

void UAbilityAttributeSet::OnRep_MontageIndex(const FGameplayAttributeData& OldMontageIndex)
{
}
