// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Abilities/Attributes/WeaponAttributeSet.h"
#include "GameFramework/Actor.h"
#include "Core/Types/PawnTypes.h"
#include "Types/EnumTypes.h"
#include "BaseWeapon.generated.h"

UCLASS(Abstract)
class ROGUELIKE_API ABaseWeapon : public AActor {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    FName EndSocket{};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    FName StartSocket{};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    FName AttachSocket{};

public:
    // Sets default values for this actor's properties
    ABaseWeapon();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class USkeletalMeshComponent> MeshComponent {};

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EOverlayState OverlayState { EOverlayState::Default };

    UPROPERTY()
    TObjectPtr<UWeaponAttributeSet> Attributes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponType WeaponType { EWeaponType::OneHanded };

public:
    UFUNCTION(BlueprintGetter)
    FORCEINLINE USkeletalMeshComponent* GetMesh()
    {
        return MeshComponent;
    };

    UFUNCTION(BlueprintGetter)
    FORCEINLINE EWeaponType GetWeaponType()
    {
        return WeaponType;
    };

    void SetVisibility(bool NewVisible) const;

    UFUNCTION(BlueprintPure)
    FORCEINLINE EOverlayState GetOverlayState() const
    {
        return OverlayState;
    };

    UFUNCTION(BlueprintGetter)
    FORCEINLINE float GetBaseDamage() const
    {
        return Attributes->GetBaseDamage();
    }
    UFUNCTION(BlueprintGetter)
    FORCEINLINE float GetDamageMultiplier() const
    {
        return Attributes->GetDamageMultiplier();
    }
    UFUNCTION(BlueprintGetter)
    FORCEINLINE TArray<TSubclassOf<UGameplayEffect>> GetGameplayEffects()
    {
        return GameplayEffects;
    }

    UFUNCTION(BlueprintGetter)
    FORCEINLINE FName GetAttachSocket()
    {
        return AttachSocket;
    }

    virtual FVector GetStartPoint();
    virtual FVector GetEndPoint();
};
