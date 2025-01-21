// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PawnAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class ROGUELIKE_API UPawnAttributeSet : public UAttributeSet {
    GENERATED_BODY()

public:
    UPawnAttributeSet() = default;

    // AttributeSet Overrides
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", ReplicatedUsing = OnRep_MontageIndex)
    FGameplayAttributeData MontageIndex { 0 };
    ATTRIBUTE_ACCESSORS(ThisClass, MontageIndex)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health { 1.f };
    ATTRIBUTE_ACCESSORS(ThisClass, Health)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth { 1.f };
    ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_Defence)
    FGameplayAttributeData Defence { .03f };
    ATTRIBUTE_ACCESSORS(ThisClass, Defence)

    /** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into
     * -Health */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    FGameplayAttributeData ReceivedDamage;
    ATTRIBUTE_ACCESSORS(ThisClass, ReceivedDamage)

    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_Strength)
    FGameplayAttributeData Strength;
    ATTRIBUTE_ACCESSORS(ThisClass, Strength)

protected:
    APawn* GetTargetPawn(const FGameplayEffectModCallbackData& Data);

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
    virtual void OnRep_MontageIndex(const FGameplayAttributeData& OldMontageIndex);

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);

    UFUNCTION()
    virtual void OnRep_Defence(const FGameplayAttributeData& OldDefence);
};

inline void UPawnAttributeSet::OnRep_MontageIndex(const FGameplayAttributeData& OldMontageIndex)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MontageIndex, OldMontageIndex);
}

inline void UPawnAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldHealth);
}

inline void UPawnAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldMaxHealth);
}

inline void UPawnAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Strength, OldStrength);
}

inline void UPawnAttributeSet::OnRep_Defence(const FGameplayAttributeData& OldDefence)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Defence, OldDefence);
}