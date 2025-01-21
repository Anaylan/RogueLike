// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Player/BasePlayerState.h"
#include "Core/Abilities/Attributes/PawnAttributeSet.h"
#include "Components/BaseAbilitySystemComponent.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPawnAttributeSet>(TEXT("AttributeSet"));

	SetNetUpdateFrequency(100.0f);
}

float ABasePlayerState::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float ABasePlayerState::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float ABasePlayerState::GetReceivedDamage() const
{
	return AttributeSet->GetReceivedDamage();
}

float ABasePlayerState::GetStrength() const
{
	return AttributeSet->GetStrength();
}

bool ABasePlayerState::IsAlive() const
{
	return GetHealth() > 0.f;
}
