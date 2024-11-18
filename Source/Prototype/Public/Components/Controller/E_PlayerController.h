// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "E_PlayerController.generated.h"


struct FInputActionValue;
/**
 * 
 */
UCLASS()
class PROTOTYPE_API AE_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/****************
	* Input Actions *
	****************/
	UFUNCTION(BlueprintCallable)
	void IA_Move(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void IA_Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void IA_Jump(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void IA_Sprint(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void IA_Walk(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void IA_Rotation(const FInputActionValue& Value);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	virtual void OnRep_Owner() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<class AB_Character> PossessedPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<class UInputMappingContext> DefaultInputMapping{};

	void SetupInput();

	void BindActions(UInputMappingContext* InputMapping);
private:

};
