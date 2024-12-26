// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GASPCharacter.h"
#include "AbilitySystemInterface.h"
#include "Core/Interfaces/WeaponInterface.h"
#include "Core/Types/AbilityTypes.h"
#include "Core/Types/PawnTypes.h"
#include "PlayableCharacter.generated.h"

enum class EAbilityInputID : uint8;
/**
 * 
 */
UCLASS()
class PROTOTYPE_API APlayableCharacter : public AGASPCharacter, public IAbilitySystemInterface, public IWeaponInterface
{
	GENERATED_BODY()

	uint8 bAbilitiesInitialized:1;

	// friend UHopperAttributeSet;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TMap<EAbilityInputID, TSubclassOf<class UGameplayAbility>> GameplayAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> PassiveGameplayEffects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	EOverlayState OverlayState{ EOverlayState::Default };
	
	virtual void AddStartupGameplayAbilities();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> FollowCamera{ };
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> CameraBoom{ };

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<ABaseWeapon> CurrentWeapon{ };
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponSocket{ TEXT("Weapon_R")};
	UPROPERTY(EditDefaultsOnly, meta=(AllowAbstract = false))
	TArray<TSubclassOf<ABaseWeapon>> Weapons;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated)
	TArray<TObjectPtr<ABaseWeapon>> SpawnedWeapons;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int32 WeaponIndex{ 0 };

	UFUNCTION()
	void OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon);
	void SpawnWeapons();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:

	UFUNCTION(BlueprintGetter)
	FORCEINLINE EOverlayState GetOverlayState() const { return OverlayState; }
	
	explicit APlayableCharacter(const FObjectInitializer& ObjectInitializer);

	virtual bool HasWeapon() override;
	virtual ABaseWeapon* GetCurrentWeapon() override { return CurrentWeapon; };

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable)
	void TryUseAbility(EAbilityInputID AbilityInputID);

	void SetOverlayState(EOverlayState NewOverlayState, bool bForce = false);
	UFUNCTION(Server, Reliable)
	void Server_SetOverlayState(EOverlayState NewOverlayState);

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual bool CanAttack() const;
};
