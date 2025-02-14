// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_BaseSweepTrace.h"
#include "AnimNotifyState_BoneMeleeTrace.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Bone Sweep Trace")
class ROGUELIKE_API UAnimNotifyState_BoneMeleeTrace : public UAnimNotifyState_BaseSweepTrace
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FBoneReference BoneReference;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SphereRadius{15.f};

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
};
