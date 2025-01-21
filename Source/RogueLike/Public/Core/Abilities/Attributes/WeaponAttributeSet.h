// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "WeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class ROGUELIKE_API UWeaponAttributeSet : public UAttributeSet {
    GENERATED_BODY()

public:
    UWeaponAttributeSet() = default;

    // AttributeSet Overrides
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_DamageMultiplier)
    FGameplayAttributeData DamageMultiplier { 0 };
    ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, DamageMultiplier)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_BaseDamage)
    FGameplayAttributeData BaseDamage { 1.f };
    ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, BaseDamage)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_AttackSpeed)
    FGameplayAttributeData AttackSpeedBonus { 1.f };
    ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, AttackSpeedBonus)

protected:
    // Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
    // (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
    void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
        const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
        const FGameplayAttribute& AffectedAttributeProperty);

    /**
     * These OnRep functions exist to make sure that the ability system internal representations are synchronized
     *properly during replication
     **/

    UFUNCTION()
    virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage);

    UFUNCTION()
    virtual void OnRep_DamageMultiplier(const FGameplayAttributeData& OldDamageMultiplier);

    UFUNCTION()
    virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed);
};

inline void UWeaponAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeedBonus)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeedBonus, OldAttackSpeedBonus)
}

inline void UWeaponAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseDamage, OldBaseDamage);
}

inline void UWeaponAttributeSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldDamageMultiplier)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamageMultiplier, OldDamageMultiplier);
}