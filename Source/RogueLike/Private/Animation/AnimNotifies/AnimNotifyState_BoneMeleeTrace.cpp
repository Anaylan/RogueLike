// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotifyState_BoneMeleeTrace.h"

void UAnimNotifyState_BoneMeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 const float FrameDeltaTime,
                                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	const FVector StartLocation{MeshComp->GetBoneLocation(BoneReference.BoneName)};
	const FVector EndLocation{MeshComp->GetBoneLocation(BoneReference.BoneName)};

	PerformSweep(StartLocation, EndLocation, FVector(SphereRadius), StartLocation.ToOrientationQuat());
}
