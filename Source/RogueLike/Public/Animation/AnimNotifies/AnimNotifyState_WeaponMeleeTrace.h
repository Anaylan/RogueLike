// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_BaseSweepTrace.h"
#include "Core/Types/PawnTypes.h"
#include "AnimNotifyState_WeaponMeleeTrace.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Melee Weapon Sweep Trace")
class ROGUELIKE_API UAnimNotifyState_WeaponMeleeTrace : public UAnimNotifyState_BaseSweepTrace
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag WeaponSlot;

	UPROPERTY()
	TWeakObjectPtr<class AMeleeWeapon> Weapon;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
};
