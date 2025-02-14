// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


struct FGameplayAbilityTargetDataHandle;
/**
 * 
 */
class ROGUELIKE_API FMotionWarpingHelper
{
public:
	static void ApplyMotionWarping(const AActor* AvatarActor, const FName WarpTargetName, const FVector& TargetLocation,
	                               const FVector& Offset);

	/**
	 * Finds closest actor ot the filtered class for Motion Warping.
	 * @param AvatarActor The actor relative to which the target is searched.
	 * @param FilteredClass The class of actors participating in the search.
	 * @return The location of the nearest actor or FVector::ZeroVector if not found.
	 */
	static FVector FindNearestMotionWarpTarget(const AActor* AvatarActor, const TSubclassOf<AActor> FilteredClass);
};
