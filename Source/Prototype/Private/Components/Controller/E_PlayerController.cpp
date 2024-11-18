// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Controller/E_PlayerController.h"
#include "Actors/Characters/B_Character.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Core/Types/EnumTypes.h"

void AE_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPlayer = Cast<AB_Character>(InPawn);

	if (PossessedPlayer && PossessedPlayer->GetAbilitySystemComponent())
	{
		PossessedPlayer->GetAbilitySystemComponent()->InitAbilityActorInfo(PossessedPlayer, InPawn);
	}

	SetupInput();
}

void AE_PlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	PossessedPlayer = Cast<AB_Character>(GetPawn());

	SetupInput();
}

void AE_PlayerController::OnRep_Owner()
{
	Super::OnRep_Owner();

	PossessedPlayer = Cast<AB_Character>(GetOwner());

	SetupInput();
}

void AE_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) 
	{
		EnhancedInputComponent->ClearActionBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();
		EnhancedInputComponent->ClearAxisBindings();

		BindActions(DefaultInputMapping.LoadSynchronous());

		if (PossessedPlayer && PossessedPlayer->GetAbilitySystemComponent())
		{
			const FGameplayAbilityInputBinds Binds(
				"Confirm",
				"Cancel",
				FTopLevelAssetPath(GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>(
					"/Script/Prototype.EAbilityInputID"))),
				StaticCast<int32>(EAbilityInputID::Confirm),
				StaticCast<int32>(EAbilityInputID::Cancel));

			PossessedPlayer->GetAbilitySystemComponent()->BindAbilityActivationToInputComponent(InputComponent, Binds);
		}
	}
}

void AE_PlayerController::SetupInput()
{
	if (!PossessedPlayer) return;
	// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
		InputSystem->ClearAllMappings();

		FModifyContextOptions Options;
		Options.bForceImmediately = true;

		// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
		InputSystem->AddMappingContext(DefaultInputMapping.LoadSynchronous(), 1, Options);
	}
}

void AE_PlayerController::BindActions(UInputMappingContext* InputMapping)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInputComponent) || !IsValid(InputMapping)) return;

	TArray<FEnhancedActionKeyMapping> KeyMappings = InputMapping->GetMappings();
	if (KeyMappings.IsEmpty()) return;

	TSet<const UInputAction*> Actions;
	for (auto& KeyMapping : KeyMappings)
	{
		Actions.Add(KeyMapping.Action);
	}
	
	for (auto* Action : Actions)
	{
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered,
			Cast<UObject>(this), Action->GetFName());
	}
}

void AE_PlayerController::IA_Move(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer)) 
	{
		PossessedPlayer->MoveAction(Value.Get<FVector2D>());
	}
}

void AE_PlayerController::IA_Look(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer))
	{
		PossessedPlayer->LookAction(Value.Get<FVector2D>());
	}
}

void AE_PlayerController::IA_Jump(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer))
	{
		PossessedPlayer->TryUseAbility(EAbilityInputID::IA_Jump);
	}
}

void AE_PlayerController::IA_Sprint(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer))
	{
		PossessedPlayer->TryUseAbility(EAbilityInputID::IA_Sprint);
	}
}

void AE_PlayerController::IA_Walk(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer))
	{
		PossessedPlayer->TryUseAbility(EAbilityInputID::IA_Walk);
	}
}

void AE_PlayerController::IA_Rotation(const FInputActionValue& Value)
{
	if (IsValid(PossessedPlayer))
	{
		PossessedPlayer->TryUseAbility(EAbilityInputID::IA_Rotation);
	}
}