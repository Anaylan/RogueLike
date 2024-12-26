// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PawnAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class PROTOTYPE_API UPawnAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UPawnAttributeSet() = default;

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_MontageIndex)
	FGameplayAttributeData MontageIndex{0};
	ATTRIBUTE_ACCESSORS(ThisClass, MontageIndex)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_PlayRate)
	FGameplayAttributeData PlayRate{1.f};
	ATTRIBUTE_ACCESSORS(ThisClass, PlayRate)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_PlayRate)
	FGameplayAttributeData HealthPoints{1.f};
	ATTRIBUTE_ACCESSORS(ThisClass, HealthPoints)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", ReplicatedUsing = OnRep_PlayRate)
	FGameplayAttributeData MaxHealthPoints{1.f};
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealthPoints)

	/** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into -Health */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData CausedDamage;
	ATTRIBUTE_ACCESSORS(ThisClass, CausedDamage)
	
protected:

	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/**
	* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	**/
	UFUNCTION()
	virtual void OnRep_PlayRate(const FGameplayAttributeData& OldPlayRate);

	UFUNCTION()
	virtual void OnRep_MontageIndex(const FGameplayAttributeData& OldMontageIndex);

	UFUNCTION()
	virtual void OnRep_HealthPoints(const FGameplayAttributeData& OldHealthPoints);

	UFUNCTION()
	virtual void OnRep_MaxHealthPoints(const FGameplayAttributeData& OldMaxHealthPoints);
};
