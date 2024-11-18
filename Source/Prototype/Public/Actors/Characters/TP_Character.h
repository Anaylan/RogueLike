// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/B_Character.h"
#include "Core/Interfaces/ActionInterface.h"
#include "TP_Character.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API ATP_Character : public AB_Character, public IActionInterface
{
	GENERATED_BODY()
	
public:

	explicit ATP_Character(const FObjectInitializer& ObjectInitializer);

	bool HasWeapon();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> FollowCamera{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> CameraBoom{};

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<class ABaseWeapon> CurrentWeapon{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponSocket{ TEXT("Weapon_R")};

	UPROPERTY(EditDefaultsOnly, meta=(AllowAbstract = false))
	TArray<TSubclassOf<ABaseWeapon>> Weapons;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated)
	TArray<TObjectPtr<ABaseWeapon>> SpawnedWeapons;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int32 WeaponIndex = 0;

	virtual void EquipAction() override;
	virtual void AttackAction() override;

	UFUNCTION()
	void OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon);

	void SpawnWeapons();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
