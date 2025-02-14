// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Helpers/MotionWarpingHelper.h"
#include "MotionWarpingComponent.h"
#include "Core/Helpers/CommonHelper.h"
#include "Kismet/GameplayStatics.h"

void FMotionWarpingHelper::ApplyMotionWarping(const AActor* AvatarActor, const FName WarpTargetName,
                                              const FVector& TargetLocation, const FVector& Offset)
{
	if (UMotionWarpingComponent* MotionWarpingComp = AvatarActor->FindComponentByClass<UMotionWarpingComponent>())
	{
		const FVector FinalLocation = TargetLocation + Offset;

		FMotionWarpingTarget WarpTarget;
		WarpTarget.Name = WarpTargetName;
		WarpTarget.Location = FinalLocation;
		WarpTarget.Rotation = (FinalLocation - AvatarActor->GetActorLocation()).Rotation();

		MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);
	}
#if WITH_EDITOR
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MotionWarpingComponent not found on actor!"));
	}
#endif
}

FVector FMotionWarpingHelper::FindNearestMotionWarpTarget(const AActor* AvatarActor,
                                                          const TSubclassOf<AActor> FilteredClass)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(AvatarActor->GetWorld(), FilteredClass, FoundActors);
	if (FoundActors.IsEmpty())
	{
		return FVector::ZeroVector;
	}

	const AActor* ClosestActor = FCommonHelper::FindClosestActor(FoundActors, AvatarActor);
	return IsValid(ClosestActor) ? ClosestActor->GetActorLocation() : FVector::ZeroVector;
}
