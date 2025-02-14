// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayableCharacter.generated.h"

UCLASS()
class ROGUELIKE_API APlayableCharacter : public ABaseCharacter 
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UGameplayCameraComponent> GameplayCamera{};

public:
    APlayableCharacter(const FObjectInitializer& ObjectInitializer);
    APlayableCharacter() = default;

    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;
    virtual void OnRep_Controller() override;
};
