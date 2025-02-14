// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GASPCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Core/Types/AbilityTypes.h"
#include "Components/RLAbilitySystemComponent.h"
#include "Core/Interfaces/WeaponInterface.h"
#include "BaseCharacter.generated.h"

enum class EAbilityInputID : uint8;
/**
 *
 */
UCLASS(Abstract)
class ROGUELIKE_API ABaseCharacter : public AGASPCharacter, public IAbilitySystemInterface, public IWeaponInterface
{
	GENERATED_BODY()

	uint8 bAbilitiesInitialized : 1 {false};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DeadTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<URLAbilitySystemComponent> AbilitySystemComponent{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> GameplayAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UPawnAttributeSet> AttributeSet{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	TObjectPtr<class UWeaponComponent> WeaponComponent{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TMap<FGameplayTag, TObjectPtr<class UAbilityAnimSetDataAsset>> AbilitiesAnimAsset;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Ability")
	int32 MontageIndex;

	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	ABaseCharacter() = default;


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UPawnAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable)
	void TryUseAbility(const EAbilityInputID AbilityInputID, const bool bPressed);

	virtual void OnHealthChanged(const FOnAttributeChangeData& Data)
	{
	}

	virtual void OnDamaged(const FOnAttributeChangeData& Data)
	{
	}

	virtual void Jump() override;

	virtual bool HasWeapons() override;
	virtual bool SwitchWeapon(const FGameplayTag& WeaponSlot, int32 Index) const override;
	virtual int32 GetEquippedWeaponsSize() const override;

	virtual ABaseWeapon* GetWeaponInSlot(const FGameplayTag Slot) override;

	UFUNCTION(BlueprintPure)
	UAbilityAnimSetDataAsset* GetAbilitiesAnimAsset(const FGameplayTag SearchTag) const;

	UFUNCTION(BlueprintCallable)
	void GiveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& GameplayEffects) const;
	UFUNCTION(BlueprintCallable)
	void RemoveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& EffectsToRemove) const;
	UFUNCTION(BlueprintCallable)
	void GiveGameplayAbilities(TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> AbilitiesToGive);
	UFUNCTION(BlueprintCallable)
	void RemoveGameplayAbility(const UGameplayAbility* AbilityToRemove) const;
};

inline UAbilityAnimSetDataAsset* ABaseCharacter::GetAbilitiesAnimAsset(const FGameplayTag SearchTag) const
{
	return AbilitiesAnimAsset.Contains(SearchTag) ? *AbilitiesAnimAsset.Find(SearchTag) : nullptr;
}

inline UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return static_cast<UAbilitySystemComponent*>(AbilitySystemComponent);
}

inline UPawnAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}
