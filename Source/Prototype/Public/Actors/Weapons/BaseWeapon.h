// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS(Abstract)
class PROTOTYPE_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> Mesh{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOverlayState OverlayState{};
	
public:	

	UFUNCTION(BlueprintGetter)
	FORCEINLINE USkeletalMeshComponent* GetMesh() { return Mesh; };
	
	void AttachTo(AActor* NewOwner);
	void Detach();

	void Visible(bool NewVisible);
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE EOverlayState GetOverlayState() const { return OverlayState; };
};
