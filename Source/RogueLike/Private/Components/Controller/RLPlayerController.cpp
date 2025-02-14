// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Controller/RLPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Core/Types/AbilityTypes.h"

ARLPlayerController::ARLPlayerController()
{
}

void ARLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPlayer = Cast<APlayableCharacter>(InPawn);

	if (PossessedPlayer && PossessedPlayer->GetAbilitySystemComponent())
	{
		PossessedPlayer->GetAbilitySystemComponent()->InitAbilityActorInfo(PossessedPlayer, InPawn);
	}

	SetupInput();
}

void ARLPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	PossessedPlayer = Cast<APlayableCharacter>(GetPawn());

	SetupInput();
}

void ARLPlayerController::OnRep_Owner()
{
	Super::OnRep_Owner();

	PossessedPlayer = Cast<APlayableCharacter>(GetOwner());

	SetupInput();
}

void ARLPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsValid(PossessedPlayer))
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent())
	{
		const FGameplayAbilityInputBinds Binds(
			FString("Confirm"), FString("Cancel"),
			FTopLevelAssetPath(GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>("EAbilityInputID"))),
			static_cast<int32>(EAbilityInputID::Confirm), static_cast<int32>(EAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void ARLPlayerController::SetupInput() const
{
	if (!IsValid(PossessedPlayer))
	{
		return;
	}
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
