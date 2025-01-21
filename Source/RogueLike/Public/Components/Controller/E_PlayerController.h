// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "E_PlayerController.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API AE_PlayerController : public APlayerController 
{
    GENERATED_BODY()

public:
    AE_PlayerController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnRep_Pawn() override;
    virtual void OnRep_Owner() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<class APlayableCharacter> PossessedPlayer{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class UInputMappingContext> DefaultInputMapping{};

    void SetupInput() const;
};
