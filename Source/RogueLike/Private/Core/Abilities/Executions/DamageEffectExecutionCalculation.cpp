// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/Executions/DamageEffectExecutionCalculation.h"

#include "Core/Abilities/Attributes/PawnAttributeSet.h"

struct FDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ReceivedDamage);

	FDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPawnAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPawnAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPawnAttributeSet, Defence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPawnAttributeSet, ReceivedDamage, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UPawnAttributeSet, Strength, Source, false);
	}
};

static FDamageCapture& GetDamageCapture()
{
	static FDamageCapture DamageCapture;
	return DamageCapture;
};

UDamageEffectExecutionCalculation::UDamageEffectExecutionCalculation()
{
	RelevantAttributesToCapture.Add(GetDamageCapture().HealthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().MaxHealthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().StrengthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DefenceDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().ReceivedDamageDef);
}

void UDamageEffectExecutionCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;

	float Health{0.f};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().HealthDef, EvaluateParams, Health);
	float MaxHealth{0.f};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().MaxHealthDef, EvaluateParams,
	                                                           MaxHealth);
	float Defence{0.f};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DefenceDef, EvaluateParams, Defence);

	float Strength{0.f};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().StrengthDef, EvaluateParams,
	                                                           Strength);

	float ArmorPenetration{0.f};
	// HPDefenderResult = (HPDefender + DefenceDefender) - DamageAttacker
	// TODO: Bring OutDamage to standard
	// Standard: DMG = DMG * (1 - (Defence / (Defence + Modifiers)))
	// Apply armor penetration
	float EffectiveDefence = FMath::Max(Defence * (1.f - ArmorPenetration), 0.f);
    
	// Improved damage reduction formula with diminishing returns
	float ReducedDamage = FMath::Max(1.f - (EffectiveDefence / (EffectiveDefence + 100.f + (EffectiveDefence * 0.1f))), 0.1f);
    
	// Base damage calculation
	const float OutDamage = FMath::Max(Strength * ReducedDamage, 0.f);

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(GetDamageCapture().ReceivedDamageProperty, EGameplayModOp::Additive, OutDamage));
}
