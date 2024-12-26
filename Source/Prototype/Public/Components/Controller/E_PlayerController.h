// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "E_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AE_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	virtual void OnRep_Owner() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<class APlayableCharacter> PossessedPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<class UInputMappingContext> DefaultInputMapping{};

	void SetupInput() const;
};
