// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SweepMultiByChannel.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(DisplayName="SweepMultiByChannel")
class PROTOTYPE_API UAnimNotifyState_SweepMultiByChannel : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	TSet<TObjectPtr<AActor>> OverlappedActors;
	
public:
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	
	void SweepMultiByChannel(APawn* Owner, const FVector& StartLocation, const FVector& EndLocation,
		UAbilitySystemComponent* AbilitySystemComponent, const FCollisionQueryParams& CollisionQueryParams);
};
