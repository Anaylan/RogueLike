// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Controller/E_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Core/Types/AbilityTypes.h"
#include "Components/Manager/E_PlayerCameraManager.h"

AE_PlayerController::AE_PlayerController()
{
}

void AE_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPlayer = Cast<APlayableCharacter>(InPawn);

	if (PossessedPlayer && PossessedPlayer->GetAbilitySystemComponent())
	{
		PossessedPlayer->GetAbilitySystemComponent()->InitAbilityActorInfo(PossessedPlayer, InPawn);
	}

	SetupInput();
}

void AE_PlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	PossessedPlayer = Cast<APlayableCharacter>(GetPawn());

	SetupInput();
}

void AE_PlayerController::OnRep_Owner()
{
	Super::OnRep_Owner();

	PossessedPlayer = Cast<APlayableCharacter>(GetOwner());

	SetupInput();
}

void AE_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsValid(PossessedPlayer))
		return;

	if (UAbilitySystemComponent* AbilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent())
	{
		const FGameplayAbilityInputBinds Binds(
			FString("Confirm"), FString("Cancel"),
			FTopLevelAssetPath(GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>("EAbilityInputID"))),
			StaticCast<int32>(EAbilityInputID::Confirm), StaticCast<int32>(EAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AE_PlayerController::SetupInput() const
{
	if (!IsValid(PossessedPlayer))
		return;
	// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
	if (UEnhancedInputLocalPlayerSubsystem* InputSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover
		// mappings.
		InputSystem->ClearAllMappings();

		FModifyContextOptions Options;
		Options.bForceImmediately = true;

		// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
		InputSystem->AddMappingContext(DefaultInputMapping.LoadSynchronous(), 1, Options);
	}
}
