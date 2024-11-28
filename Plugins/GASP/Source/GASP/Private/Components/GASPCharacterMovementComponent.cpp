// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/B_CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UB_CharacterMovementComponent::UB_CharacterMovementComponent()
{
	RotationRate = FRotator(0.f, -1.f, 0.f);
	BrakingDecelerationWalking = 1500.f;
	MinAnalogWalkSpeed = 150.f;
	MaxWalkSpeed = 500.f;
	GroundFriction = 5.f;
	BrakingFrictionFactor = 1.f;
	BrakingFriction = 0.f;
	MaxAcceleration = 800.f;
	PerchRadiusThreshold = 20.0f;
	bUseFlatBaseForFloorChecks = true;
	bUseSeparateBrakingFriction = true;
}

bool UB_CharacterMovementComponent::FSavedMove_Base::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_Base* NewCombineMove = StaticCast<FSavedMove_Base*>(NewMove.Get());

	if (bSavedRotationModeUpdate != NewCombineMove->bSavedRotationModeUpdate)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UB_CharacterMovementComponent::FSavedMove_Base::Clear()
{
	Super::Clear();

	SavedGait = EGait::Run;
	bSavedRotationModeUpdate = false;
	SavedRotationMode = ERotationMode::OrientToMovement;
}

uint8 UB_CharacterMovementComponent::FSavedMove_Base::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedRotationModeUpdate)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

void UB_CharacterMovementComponent::FSavedMove_Base::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const UB_CharacterMovementComponent* CharacterMovement = Cast<UB_CharacterMovementComponent>(C->GetCharacterMovement());

	if (CharacterMovement)
	{
		SavedGait = CharacterMovement->SafeGait;
		bSavedRotationModeUpdate = CharacterMovement->bSafeRotationModeUpdate;
		SavedRotationMode = CharacterMovement->SafeRotationMode;
	}
}

void UB_CharacterMovementComponent::FSavedMove_Base::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UB_CharacterMovementComponent* CharacterMovement = Cast<UB_CharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->SafeGait = SavedGait;
	CharacterMovement->SafeRotationMode = SavedRotationMode;
}

UB_CharacterMovementComponent::FNetworkPredictionData_Client_Base::FNetworkPredictionData_Client_Base(
	const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UB_CharacterMovementComponent::FNetworkPredictionData_Client_Base::AllocateNewMove()
{
	return MakeShared<FSavedMove_Base>();
}

FNetworkPredictionData_Client* UB_CharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (!ClientPredictionData)
	{
		UB_CharacterMovementComponent* MutableThis = const_cast<UB_CharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Base(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UB_CharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bSafeRotationModeUpdate = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}


void UB_CharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (!CharacterOwner) return;

	if (IsMovingOnGround())
	{
		if (bSafeRotationModeUpdate)
		{
			UpdateRotationMode();
		}
		
		MaxWalkSpeed = GaitSettings.GetMappedSpeed(SafeGait, Velocity, GetActorTransform());
	}
}

void UB_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	RotationRate = FRotator(0.f,  IsFalling() ? 200.f : -1.f, 0.f);
}

float UB_CharacterMovementComponent::CalculateMaxAcceleration()
{
	if (SafeGait == EGait::Run || SafeGait == EGait::Walk) return MovementInformation.MaxAcceleration;
	
	return FMath::GetMappedRangeValueClamped<float, float>({ 300.f, StaticCast<float>(GaitSettings.GetSpeedForGait(EGait::Sprint).X) },
		{ MovementInformation.MaxAcceleration, 300.f }, Velocity.Size2D());
}

float UB_CharacterMovementComponent::CalculateGroundFriction()
{
	if (SafeGait == EGait::Run || SafeGait == EGait::Walk) return MovementInformation.GroundFriction;
	
	return FMath::GetMappedRangeValueClamped<float, float>({ 0.f, StaticCast<float>(GaitSettings.GetSpeedForGait(EGait::Run).X) },
		{ MovementInformation.GroundFriction, 3.f }, Velocity.Size2D());
}

float UB_CharacterMovementComponent::CalculateBrakingDecelerationWalking()
{
	return HasMovementInputVector() ? MovementInformation.MovingMaxBrakingDeceleration :
		MovementInformation.NotMovingMaxBrakingDeceleration;
}

void UB_CharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	MaxAcceleration = CalculateMaxAcceleration();
	BrakingDecelerationWalking = CalculateBrakingDecelerationWalking();
	
	GroundFriction = CalculateGroundFriction();
	
	Super::PhysWalking(deltaTime, Iterations);
}

bool UB_CharacterMovementComponent::HasMovementInputVector()
{
	return GetPendingInputVector() != FVector::ZeroVector;
}

void UB_CharacterMovementComponent::UpdateRotationMode()
{
	switch (SafeRotationMode)
	{
	case ERotationMode::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		break;
	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		break;
	}
}

void UB_CharacterMovementComponent::Server_SetGait_Implementation(EGait NewGait)
{
	SafeGait = NewGait;
}

void UB_CharacterMovementComponent::SetGait(EGait NewGait)
{
	if (NewGait != SafeGait)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			SafeGait = NewGait;
			if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Server_SetGait(NewGait);
			}
		}
	}
}

void UB_CharacterMovementComponent::SetRotationMode(ERotationMode NewRotationMode)
{
	if (NewRotationMode != SafeRotationMode)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			SafeRotationMode = NewRotationMode;
			if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Server_SetRotationMode(NewRotationMode);
			}
			bSafeRotationModeUpdate = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			UpdateRotationMode();
		}
	}
}

void UB_CharacterMovementComponent::Server_SetRotationMode_Implementation(ERotationMode NewRotationMode)
{
	SafeRotationMode = NewRotationMode;
}
