// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GASPCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Core/Interfaces/WeaponInterface.h"
#include "Core/Types/AbilityTypes.h"
#include "Core/Types/PawnTypes.h"
#include "Components/BaseAbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

enum class EAbilityInputID : uint8;
/**
 *
 */
UCLASS()
class ROGUELIKE_API ABaseCharacter : public AGASPCharacter, public IAbilitySystemInterface, public IWeaponInterface {
    GENERATED_BODY()

    uint8 bAbilitiesInitialized : 1 { false };

    // friend UPawnAttributeSet

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag DeadTag;

    UPROPERTY()
    TObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> GameplayAbilities;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> PassiveGameplayEffects;
    UPROPERTY()
    TObjectPtr<class UPawnAttributeSet> AttributeSet;

    UFUNCTION(BlueprintCallable)
    void GiveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& GameplayEffects) const;
    UFUNCTION(BlueprintCallable)
    void RemoveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& EffectsToRemove) const;
    UFUNCTION(BlueprintCallable)
    void GiveGameplayAbilities(TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> AbilitiesToGive);
    UFUNCTION(BlueprintCallable)
    void RemoveGameplayAbility(const UGameplayAbility* AbilityToRemove);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
    TObjectPtr<ABaseWeapon> CurrentWeapon {};
    // or in meta replace BlueprintBaseOnly to AllowAbstract = false
    UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
    TArray<TSubclassOf<ABaseWeapon>> Weapons;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated)
    TArray<TObjectPtr<ABaseWeapon>> SpawnedWeapons;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 LeftWeaponIndex { 0 };
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RightWeaponIndex { 0 };

public:
    ABaseCharacter(const FObjectInitializer& ObjectInitializer);
    ABaseCharacter() = default;

    virtual bool HasWeapon(EWeaponSlot WeaponSlot = EWeaponSlot::None) override;
    virtual ABaseWeapon* GetCurrentWeapon(EWeaponSlot WeaponSlot = EWeaponSlot::None) override
    {
        if (WeaponSlot == EWeaponSlot::None)
            return nullptr;

        return CurrentWeapon;
    };

    UFUNCTION()
    virtual void OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon);
    UFUNCTION()
    virtual void SpawnWeapons();

    virtual bool CanAttack() const override;
    virtual bool CanEquipWeapon() const override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual UPawnAttributeSet* GetAttributeSet() const;

    UFUNCTION(BlueprintCallable)
    void TryUseAbility(EAbilityInputID AbilityInputID, bool bPressed);

    virtual void OnHealthChanged(const FOnAttributeChangeData& Data) {};
    virtual void OnDamaged(const FOnAttributeChangeData& Data) {};
};

inline UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
    return StaticCast<UAbilitySystemComponent*>(AbilitySystemComponent);
}

inline UPawnAttributeSet* ABaseCharacter::GetAttributeSet() const
{
    return AttributeSet;
}

// inline void ABaseCharacter::OnDamaged(const FOnAttributeChangeData& Data) {}
//
// inline void ABaseCharacter::OnHealthChanged(const FOnAttributeChangeData& Data) {}