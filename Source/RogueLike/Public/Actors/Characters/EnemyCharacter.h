// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ROGUELIKE_API AEnemyCharacter : public ABaseCharacter {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties

    AEnemyCharacter(const FObjectInitializer& ObjectInitializer);

    AEnemyCharacter() = default;

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

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void OnHealthChanged(const FOnAttributeChangeData& Data) override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnDeath();

    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle DamagedDelegateHandle;
};
