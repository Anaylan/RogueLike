// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/WeaponComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;


	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
}


void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponComponent, Params)
}

void ABaseCharacter::GiveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& GameplayEffects) const
{
	if (!ensure(AbilitySystemComponent))
	{
		return;
	}

	for (const auto& GameplayEffect : GameplayEffects)
	{
		if (!GameplayEffect)
		{
			continue;
		}

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->
			MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}
}

void ABaseCharacter::RemoveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& EffectsToRemove) const
{
	if (!ensure(AbilitySystemComponent))
	{
		return;
	}

	for (const auto& EffectToRemove : EffectsToRemove)
	{
		if (!EffectToRemove)
		{
			continue;
		}


		FGameplayEffectQuery Query;
		Query.EffectDefinition = EffectToRemove;
		// Query.CustomMatchDelegate.BindLambda([&](const FActiveGameplayEffect& CurEffect)
		// {
		// 	return CurEffect.Spec.Def->GetClass() == EffectToRemove;
		// });
		AbilitySystemComponent->RemoveActiveEffects(Query, -1);
	}
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseCharacter::GiveGameplayAbilities(TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> AbilitiesToGive)
{
	if (!ensure(AbilitySystemComponent))
	{
		return;
	}

	if (HasAuthority() && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server
		for (auto& GameplayAbility : AbilitiesToGive)
		{
			if (!GameplayAbility.Value)
			{
				continue;
			}

			FGameplayAbilitySpec AbilitySpec(GameplayAbility.Value, 1, static_cast<int32>(GameplayAbility.Key), this);

			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}

		bAbilitiesInitialized = true;
	}
}

void ABaseCharacter::RemoveGameplayAbility(const UGameplayAbility* AbilityToRemove) const
{
	if (!ensure(AbilitySystemComponent) || !IsValid(AbilityToRemove))
	{
		return;
	}

	const FGameplayAbilitySpecHandle AbilityToRemoveSpecHandle(AbilityToRemove->GetCurrentAbilitySpecHandle());

	AbilitySystemComponent->ClearAbility(AbilityToRemoveSpecHandle);
}


void ABaseCharacter::TryUseAbility(const EAbilityInputID AbilityInputID, const bool bPressed)
{
	if (!ensure(AbilitySystemComponent))
	{
		return;
	}
	if (!GameplayAbilities.Contains(AbilityInputID) ||
		!GetMutableDefault<UGameplayAbility>(GameplayAbilities[AbilityInputID]))
	{
		return;
	}

	const TSubclassOf<UGameplayAbility> AbilityClass = GameplayAbilities[AbilityInputID];

	const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
	if (!Spec)
	{
		return;
	}

	if (bPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(Spec->InputID);
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(Spec->InputID);
	}
}

void ABaseCharacter::Jump()
{
	Super::Jump();

	// StopAnimMontage();
}

bool ABaseCharacter::HasWeapons()
{
	return true;
}

bool ABaseCharacter::SwitchWeapon(const FGameplayTag& WeaponSlot, const int32 Index)
const
{
	if (IsValid(WeaponComponent))
	{
		UE_LOG(LogTemp, Display, TEXT("WeaponSlot: %s"), *WeaponSlot.ToString())
		return WeaponComponent->RequestWeaponSwitch(WeaponSlot, Index);
	}
	return false;
}

int32 ABaseCharacter::GetEquippedWeaponsSize() const
{
	if (IsValid(WeaponComponent))
	{
		UE_LOG(LogTemp, Display, TEXT("GetEquippedWeaponsSize = %i"), WeaponComponent->GetEquippedWeapons().Num())
		return WeaponComponent->GetEquippedWeapons().Num();
	}
	return INDEX_NONE;
}

ABaseWeapon* ABaseCharacter::GetWeaponInSlot(const FGameplayTag Slot)
{
	if (IsValid(WeaponComponent))
	{
		return WeaponComponent->GetWeaponInSlot(Slot);
	}
	return nullptr;
}
