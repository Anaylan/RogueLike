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

	DOREPLIFETIME_WITH_PARAMS(ThisClass, DamageMultiplier, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, BaseDamage, Params);
}

void UAbilityAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
}

void UAbilityAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseDamage, OldBaseDamage);
}

void UAbilityAttributeSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamageMultiplier, OldDamageMultiplier);
}
