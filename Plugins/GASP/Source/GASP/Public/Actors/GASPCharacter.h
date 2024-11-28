// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types/EnumTypes.h"
#include "Types/StructTypes.h"
#include "B_Character.generated.h"

class UB_CharacterMovementComponent;

UCLASS(Abstract)
class GASP_API AB_Character : public ACharacter
{
	GENERATED_BODY()

	uint8 bAbilitiesInitialized:1;

protected:


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TMap<EAbilityInputID, TSubclassOf<class UGameplayAbility>> GameplayAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> PassiveGameplayEffects;

	virtual void AddStartupGameplayAbilities();

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	EOverlayState OverlayState{ EOverlayState::Default };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGait Gait{ EGait::Run };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FRotationMode RotationMode{ ERotationMode::OrientToMovement };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FMovementState MovementState{ EMovementState::Idle };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FMovementMode MovementMode{ ECMovementMode::OnGround };

	// Sets default values for this character's properties
	explicit AB_Character(const FObjectInitializer& ObjectInitializer);
	AB_Character() {};

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/****************************
	*		Input actions		*
	****************************/
	UFUNCTION(BlueprintCallable)
	void MoveAction(const FVector2D& Value);

	UFUNCTION(BlueprintCallable)
	void LookAction(const FVector2D& Value);

	UFUNCTION(BlueprintCallable)
	void TryUseAbility(EAbilityInputID AbilityInputID);

	UB_CharacterMovementComponent* GetBCharacterMovement() const;

	/****************************
	*		Movement States		*
	****************************/
	void SetGait(EGait NewGait, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetGait(EGait NewGait);

	void SetOverlayState(EOverlayState NewOverlayState, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetOverlayState(EOverlayState NewOverlayState);

	void SetRotationMode(ERotationMode NewRotationMode, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetRotationMode(ERotationMode NewRotationMode);

	void SetMovementMode(ECMovementMode NewMovementMode, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetMovementMode(ECMovementMode NewMovementMode);

	void SetMovementState(EMovementState NewMovementState, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetMovementState(EMovementState NewMovementState);
};
