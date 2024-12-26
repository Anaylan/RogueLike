// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Types/EnumTypes.h"
#include "Types/StructTypes.h"
#include "GASPCharacterMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct GASP_API FMovementInformation 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovingMaxBrakingDeceleration{ 500.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NotMovingMaxBrakingDeceleration{ 2000.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAcceleration{ 800.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GroundFriction{ 5.f };
};

/**
 * 
 */

UCLASS()
class GASP_API UGASPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class GASP_API FGASPCharacterNetworkMoveData final : public FCharacterNetworkMoveData
	{
		using Super = FCharacterNetworkMoveData;

	public:
		uint8 bSavedRotationModeUpdate : 1;
		EGait SavedGait{};
		ERotationMode SavedRotationMode{};
		
		virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;
	};
	
	class GASP_API FGASPCharacterNetworkMoveDataContainer final : public FCharacterNetworkMoveDataContainer
	{
	public:
		TStaticArray<FGASPCharacterNetworkMoveData, 3> MoveData;
		
		FGASPCharacterNetworkMoveDataContainer();
	};
	
	class GASP_API FGASPSavedMove final : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

	public:
		// Flags
		uint8 bSavedRotationModeUpdate : 1;
		EGait SavedGait{};
		ERotationMode SavedRotationMode{};

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC,
			const FVector& OldStartLocation) override;
	};

	class GASP_API FNetworkPredictionData_Client_Base : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Base(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	
	FGASPCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InAirRotationYaw{ 200.f };
	
	EGait SafeGait{};
	ERotationMode SafeRotationMode{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGaitSettings GaitSettings;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	bool bSafeRotationModeUpdate{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementInformation MovementInformation;

public:
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float CalculateGroundFriction();

	virtual void PhysicsRotation(float DeltaTime) override;
	virtual void PhysNavWalking(float deltaTime, int32 Iterations) override;
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool HasMovementInputVector() const;
	void UpdateRotationMode();

	void SetGait(EGait NewGait);
	UFUNCTION(Server, Reliable)
	void Server_SetGait(EGait NewGait);

	void SetRotationMode(ERotationMode NewRotationMode);
	UFUNCTION(Server, Reliable)
	void Server_SetRotationMode(ERotationMode NewRotationMode);
	
	FORCEINLINE void SetGaitSettings(const FGaitSettings& NewGaitSettings)
	{
		GaitSettings = NewGaitSettings;
	}

	FORCEINLINE EGait GetGait() const { return SafeGait; }
	FORCEINLINE ERotationMode GetRotationMode() const { return SafeRotationMode; }
	FORCEINLINE FGaitSettings GetGaitSettings() const { return GaitSettings; }

	UGASPCharacterMovementComponent();

};
