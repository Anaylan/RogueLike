// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/RLAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<URLAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UPawnAttributeSet> AttributeSet{};

public:
	ABasePlayerState();

	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	};

	FORCEINLINE UPawnAttributeSet* GetAttributeSet()
	{
		return AttributeSet;
	}

	UFUNCTION(BlueprintGetter)
	float GetHealth() const;
	UFUNCTION(BlueprintGetter)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintGetter)
	float GetReceivedDamage() const;
	UFUNCTION(BlueprintGetter)
	float GetStrength() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;
};
