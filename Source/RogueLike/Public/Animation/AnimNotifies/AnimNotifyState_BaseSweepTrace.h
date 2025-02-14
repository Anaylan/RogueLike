// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_BaseSweepTrace.generated.h"

class UAbilitySystemComponent;
/**
 *
 */
UCLASS(Abstract)
class ROGUELIKE_API UAnimNotifyState_BaseSweepTrace : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TSet<TObjectPtr<AActor>> OverlappedActors;

	UPROPERTY()
	TArray<AActor*> IgnoredActors;
	FCollisionQueryParams QueryParams;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TWeakObjectPtr<UWorld> WorldContext;
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag HitTag{FGameplayTag::RequestGameplayTag(FName("Event.Hit"))};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannel{ECC_WorldDynamic};

#if ALLOW_CONSOLE
	uint8 bDebugEnabled : 1{true};
#endif

public:
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	void PerformSweep(const FVector& StartLocation, const FVector& EndLocation, const FVector& ShapeExtent,
	                  const FQuat& Rotation);
};
