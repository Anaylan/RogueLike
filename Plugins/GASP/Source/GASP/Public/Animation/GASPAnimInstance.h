// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/MovementAnimInstanceProxy.h"
#include "Types/StructTypes.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNodeReference.h"
#include "BoneControllers/AnimNode_FootPlacement.h"
#include "BoneControllers/AnimNode_OffsetRootBone.h"
#include "BoneControllers/AnimNode_OrientationWarping.h"
#include "B_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASP_API UB_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return new FMovementAnimInstanceProxy(this);
	}

	FTimerHandle LandedTimer;

protected:

	/*************
	* References *
	*************/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<class AB_Character> CachedCharacter{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<class UB_CharacterMovementComponent> CachedMovement{};

	/******************
	* Character state *
	******************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGait Gait{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAnimCurves AnimNames{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGait EnGait{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMovementState MovementState{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotationMode RotationMode{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMovementMode MovementMode{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Information")
	FCharacterInfo CharacterInfo{};

	UPROPERTY(BlueprintReadOnly)
	FGait PreviousGait{};

	UPROPERTY(BlueprintReadOnly)
	FMovementState PreviousMovementState{};

	UPROPERTY(BlueprintReadOnly)
	FRotationMode PreviousRotationMode{};

	UPROPERTY(BlueprintReadOnly)
	FMovementMode PreviousMovementMode{};

	UPROPERTY(BlueprintReadOnly, Category = "Character Information")
	FCharacterInfo PreviousCharacterInfo{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Motion Matching")
	FMotionMatchingInfo MotionMatching{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UChooserTable> LocomotionTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FPoseSearchQueryTrajectory Trajectory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FPoseSearchTrajectoryData TrajectoryGenerationData_Idle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FPoseSearchTrajectoryData TrajectoryGenerationData_Moving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFootPlacementPlantSettings PlantSettings_Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFootPlacementPlantSettings PlantSettings_Stops;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFootPlacementInterpolationSettings InterpolationSettings_Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFootPlacementInterpolationSettings InterpolationSettings_Stops;
	
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool isStarting() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool isMoving() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool isPivoting() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool ShouldTurnInPlace() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool ShouldSpin() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool JustLanded_Light() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool JustLanded_Heavy() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool JustTraversal() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetLandVelocity() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool PlayLand() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool PlayMovingLand() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetTrajectoryTurnAngle() const;
	
	// TODO
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetFloatLeanAmount() const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FVector GetRelativeAcceleration() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	float GetYawOffset() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	float GetPitchOffset() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void UpdateMotionMatchingMovement(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void UpdateMatchingPostSelection(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void UpdateOffsetRoot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FQuat GetDesiredFacing() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void UpdateBlendStack(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta = (BluepruntThreadSafe))
	void UpdateEssentialValues(float DeltaSeconds);
	/**************
	* Aim Offsets *
	**************/
	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	bool isEnabledAO();
	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	FVector2D Get_AOValue() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeavyLandSpeedThreshold{ 700.f };
	UPROPERTY(BlueprintReadOnly)
	bool bLanded{ false };
	UPROPERTY(BlueprintReadOnly)
	int32 MMDatabaseLOD{ };
	UPROPERTY(BlueprintReadOnly)
	bool OffsetRootBoneEnabled{ };

	UFUNCTION()
	void OnLanded(const FHitResult& HitResult);

	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	EPoseSearchInterruptMode GetMatchingInterruptMode() const;

	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	EOffsetRootBoneMode GetOffsetRootRotationMode() const;

	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	EOffsetRootBoneMode GetOffsetRootTranslationMode() const;
	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	float GetOffsetRootTranslationHalfLife() const;

	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	EOrientationWarpingSpace GetOrientationWarpingSpace() const;

	UFUNCTION(BlueprintPure, meta = (BluepruntThreadSafe))
	bool IsEnableSteering() const;
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	FPoseSearchTrajectory_WorldCollisionResults CollisionResults;
public:

	virtual void NativeBeginPlay() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	virtual void UpdateCharacterInfo();

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	virtual void UpdateMotionMatchingInfo();

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void GenerateTrajectory(float DeltaSeconds);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float GetMatchingBlendTime() const;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FFootPlacementPlantSettings GetPlantSettings() const;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	FFootPlacementInterpolationSettings GetPlantInterpolationSettings() const;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	float Get_MatchingNotifyRecencyTimeOut();
};
