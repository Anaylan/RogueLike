// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.generated.h"

UCLASS(Abstract)
class ROGUELIKE_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> CollisionComponent;

public:
	// Sets default values for this actor's properties
	AMeleeWeapon();

	UFUNCTION(BlueprintGetter)
	UBoxComponent* GetCollisionComponent() { return CollisionComponent; }

	UFUNCTION(BlueprintPure)
	virtual FVector GetStartPoint();
	UFUNCTION(BlueprintPure)
	virtual FVector GetEndPoint();
};
