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

	float Health{ 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().HealthDef, EvaluateParams, Health);
	float MaxHealth{ 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().MaxHealthDef, EvaluateParams,
		MaxHealth);
	float Defence{ 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DefenceDef, EvaluateParams, Defence);

	float Strength{ 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().StrengthDef, EvaluateParams,
		Strength);

	// HPDefenderResult = (HPDefender + DefenceDefender) - DamageAttacker
	// TODO: Bring OutDamage to standard
	// Standard: DMG = DMG * (1 - (Defence / (Defence + Modifiers)))
	float OutDamage{ FMath::Clamp(Strength * (1 - Defence / 100), 0.f, UE_MAX_FLT) };

	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), OutDamage)

		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(GetDamageCapture().ReceivedDamageProperty, EGameplayModOp::Additive, OutDamage));
}
