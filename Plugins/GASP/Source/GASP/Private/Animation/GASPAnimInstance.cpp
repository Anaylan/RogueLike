// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/GASPAnimInstance.h"
#include "Actors/GASPCharacter.h"
#include "Components/GASPCharacterMovementComponent.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "Utils/AnimationUtils.h"
#include "PoseSearch/MotionMatchingAnimNodeLibrary.h"
#include "ChooserFunctionLibrary.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "AnimationWarpingLibrary.h"
#include "BlendStack/BlendStackAnimNodeLibrary.h"
#include "BoneControllers/AnimNode_FootPlacement.h"

void UGASPAnimInstance::OnLanded(const FHitResult& HitResult)
{
	bLanded = true;
	// GetWorld()->GetTimerManager().SetTimer(LandedTimer, [this]()
	// 	{ bLanded = false; }, .2f, false);
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { bLanded = false; });
}

EPoseSearchInterruptMode UGASPAnimInstance::GetMatchingInterruptMode() const
{
	return MovementMode != PreviousMovementMode || (MovementMode.IsOnGround() && (MovementState != PreviousMovementState ||
		(Gait != PreviousGait && MovementState.IsMoving()) || StanceMode != PreviousStanceMode)) ?
			EPoseSearchInterruptMode::InterruptOnDatabaseChange : EPoseSearchInterruptMode::DoNotInterrupt;
}

EOffsetRootBoneMode UGASPAnimInstance::GetOffsetRootRotationMode() const
{
	return IsSlotActive(AnimNames.AnimationSlotName) ? EOffsetRootBoneMode::Release : EOffsetRootBoneMode::Accumulate;
}

EOffsetRootBoneMode UGASPAnimInstance::GetOffsetRootTranslationMode() const
{
	if (IsSlotActive(AnimNames.AnimationSlotName))
		return EOffsetRootBoneMode::Release;
	if (MovementMode.IsOnGround())
	{
		return MovementState.IsMoving() ? EOffsetRootBoneMode::Interpolate : EOffsetRootBoneMode::Release;
	}
	if (MovementMode.IsInAir())
	{
		return EOffsetRootBoneMode::Release;
	}
	return EOffsetRootBoneMode();
}

float UGASPAnimInstance::GetOffsetRootTranslationHalfLife() const
{
	return MovementState.IsMoving() ? .3f : .1f;
}

EOrientationWarpingSpace UGASPAnimInstance::GetOrientationWarpingSpace() const
{
	return OffsetRootBoneEnabled ? EOrientationWarpingSpace::RootBoneTransform : EOrientationWarpingSpace::ComponentTransform;
}

bool UGASPAnimInstance::IsEnableSteering() const
{
	return MovementState.IsMoving() || MovementMode.IsInAir();
}

void UGASPAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UGASPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	CachedCharacter = Cast<AGASPCharacter>(TryGetPawnOwner());
	if (!CachedCharacter.IsValid())
		return;
	
	CachedMovement = CachedCharacter->FindComponentByClass<UGASPCharacterMovementComponent>();
	if (!CachedMovement.IsValid())
		return;

	CachedCharacter->LandedDelegate.AddUniqueDynamic(this, &ThisClass::OnLanded);
}

void UGASPAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!CachedCharacter.IsValid() || !CachedMovement.IsValid())
		return;

	NetRole = CachedCharacter->GetLocalRole();
	
	Gait = CachedCharacter->GetGait();
	StanceMode = CachedCharacter->GetStanceMode();
	RotationMode = CachedCharacter->GetRotationMode();
	// MovementState = isMoving() ? EMovementState::Moving : EMovementState::Idle;
	MovementState = CachedCharacter->GetMovementState();
	MovementMode = CachedCharacter->GetMovementMode();

	RefreshCVar();
	RefreshEssentialValues(DeltaSeconds);
	RefreshTrajectory(DeltaSeconds);
	RefreshMovementDirection(DeltaSeconds);
}

void UGASPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedCharacter.IsValid() || !CachedMovement.IsValid())
		return;
}

void UGASPAnimInstance::PreUpdateAnimation(float DeltaSeconds)
{
	Super::PreUpdateAnimation(DeltaSeconds);

	PreviousCharacterInfo = CharacterInfo;
	PreviousGait = Gait;
	PreviousMovementMode = MovementMode;
	PreviousRotationMode = RotationMode;
	PreviousMovementState = MovementState;
	PreviousStanceMode = StanceMode;
}

void UGASPAnimInstance::RefreshCVar()
{
	// Get console variables
	const auto &OffsetRootEnabled = IConsoleManager::Get().FindConsoleVariable(
		TEXT("a.animnode.offsetrootbone.enable"));
	const auto &MMLod = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.MMDatabaseLOD"));

	if (OffsetRootEnabled)
	{
		OffsetRootBoneEnabled = OffsetRootEnabled->GetBool();
	}
	if (MMLod)
	{
		MMDatabaseLOD = MMLod->GetInt();
	}
}

void UGASPAnimInstance::RefreshTrajectory(const float DeltaSeconds)
{
	const auto& TrajectoryData{ CharacterInfo.Speed > 0.f ? TrajectoryGenerationData_Moving : TrajectoryGenerationData_Idle };
	FPoseSearchQueryTrajectory OutTrajectory{};
	
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory(this, TrajectoryData, DeltaSeconds,
	   Trajectory, MotionMatching.PreviousDesiredYawRotation, OutTrajectory,
	   -1.f, 30, .1f, 15);

	const TArray<AActor*> IgnoredActors{};
	UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions(CachedCharacter.Get(), this,
	  OutTrajectory, true, .01f, Trajectory, CollisionResults,
	  TraceTypeQuery_MAX, false, IgnoredActors, EDrawDebugTrace::None, true,
	  150.f);

	UPoseSearchTrajectoryLibrary::GetTrajectoryVelocity(Trajectory, -.3f, -.2f,
		MotionMatching.PreviousVelocity, false);
	UPoseSearchTrajectoryLibrary::GetTrajectoryVelocity(Trajectory, .0f, .2f,
		MotionMatching.CurrentVelocity, false);
	UPoseSearchTrajectoryLibrary::GetTrajectoryVelocity(Trajectory, .4f, .5f,
		MotionMatching.FutureVelocity, false);
}

void UGASPAnimInstance::RefreshMovementDirection(float DeltaSeconds)
{
	if (MovementState.IsIdle())
		return;
	
	CharacterInfo.Direction = UAnimationUtils::CalculateDirection(CharacterInfo.Velocity.GetSafeNormal(),
		CharacterInfo.ActorTransform.Rotator());
	
	// TODO: improve movement direction
	if (RotationMode.IsOrientToMovement() || Gait.IsSprint())
	{
		MovementDirection = EMovementDirection::F;
		return;
	}
	
	MovementDirectionThreshold = GetMovementDirectionThresholds();
	
	if (CharacterInfo.Direction >= MovementDirectionThreshold.FL && CharacterInfo.Direction <= MovementDirectionThreshold.FR)
	{
		MovementDirection = EMovementDirection::F;
		return;
	}

	if (CharacterInfo.Direction >= MovementDirectionThreshold.BL && CharacterInfo.Direction <= MovementDirectionThreshold.FL)
	{
		// TODO: Implement movement direction bias (LeftFootForward, RightFootForward), return LL or LR
		MovementDirection = EMovementDirection::LL;
		return;
	}

	if (CharacterInfo.Direction >= MovementDirectionThreshold.BR && CharacterInfo.Direction <= MovementDirectionThreshold.FR)
	{
		// TODO: Implement movement direction bias (LeftFootForward, RightFootForward), return RR or RL
		MovementDirection = EMovementDirection::RR;
		return;
	}
	
	 MovementDirection = EMovementDirection::B;
}

float UGASPAnimInstance::GetMatchingBlendTime() const
{
	if (MovementMode.IsInAir())
	{
		return GetLandVelocity() > 100.f ? .15f : .5f;
	}

	return PreviousMovementMode.IsOnGround() ? .5f : .2f;
}

FFootPlacementPlantSettings UGASPAnimInstance::GetPlantSettings() const
{
	return MotionMatching.DatabaseTags.Contains(AnimNames.StopsTag) ? PlantSettings_Stops : PlantSettings_Default;
}

FFootPlacementInterpolationSettings UGASPAnimInstance::GetPlantInterpolationSettings() const
{
	return MotionMatching.DatabaseTags.Contains(AnimNames.StopsTag) ? InterpolationSettings_Stops : InterpolationSettings_Default;
}

float UGASPAnimInstance::GetMatchingNotifyRecencyTimeOut() const
{
	static constexpr float RecencyTimeOuts[] = {.2f, .2f, .16f};
	EGait TargetGait{Gait};

	return Gait >= EGait::Walk && Gait <= EGait::Sprint ? RecencyTimeOuts[StaticCast<int32>(TargetGait)] : .2f;
}

FMovementDirectionThreshold UGASPAnimInstance::GetMovementDirectionThresholds() const
{
	switch (MovementDirection)
	{
	case EMovementDirection::F:
	case EMovementDirection::B:
		return FMovementDirectionThreshold(-60.f, 60.f, -120.f, 120.f);
	case EMovementDirection::LL:
	case EMovementDirection::LR:
	case EMovementDirection::RL:
	case EMovementDirection::RR:
		if (isPivoting())
		{
			return FMovementDirectionThreshold(-60.f, 60.f, -120.f, 120.f);
		}
		// TODO: Implement wants to aim
		return BlendStackInputs.Loop ? FMovementDirectionThreshold(-60.f, 60.f, -140.f, 140.f) :
			FMovementDirectionThreshold(-40.f, 40.f, -140.f, 140.f);
	default:
		return FMovementDirectionThreshold(-60.f, 60.f, -120.f, 120.f);
	}
}

bool UGASPAnimInstance::isStarting() const
{
	return MotionMatching.FutureVelocity.Size2D() > CharacterInfo.Velocity.Size2D() + 100.f &&
		!MotionMatching.DatabaseTags.Contains(AnimNames.PivotsTag) && MovementState.IsMoving() &&
			CachedMovement.IsValid();
}

bool UGASPAnimInstance::isMoving() const
{
	return !CharacterInfo.Velocity.IsZero() && !MotionMatching.FutureVelocity.Equals(
		FVector::ZeroVector, 10.f);
}

bool UGASPAnimInstance::isPivoting() const
{
	if (MotionMatching.FutureVelocity.IsNearlyZero() || CharacterInfo.Velocity.IsNearlyZero())
	{
		return false;
	}

	const float DotProduct = FVector::DotProduct(MotionMatching.FutureVelocity.GetSafeNormal(),
		CharacterInfo.Velocity.GetSafeNormal());
	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	return AngleDegrees >= (RotationMode.IsStrafe() ? 30.f : 45.f) && MovementState.IsMoving();
}

bool UGASPAnimInstance::ShouldTurnInPlace() const
{
	const FVector ActorForward = CharacterInfo.ActorTransform.GetRotation().GetForwardVector();
	const FVector RootForward = CharacterInfo.RootTransform.GetRotation().GetForwardVector();

	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ActorForward,
																					   RootForward)));

	// TODO: Implement wants to aim
	return AngleDegrees >= CharacterInfo.MaxTurnAngle && MovementState.IsIdle();
}

bool UGASPAnimInstance::ShouldSpin() const
{
	const FVector ActorForward = CharacterInfo.ActorTransform.GetRotation().GetForwardVector();
	const FVector RootForward = CharacterInfo.RootTransform.GetRotation().GetForwardVector();

	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ActorForward,
		RootForward)));
	return AngleDegrees >= 130.f && CharacterInfo.Speed >= 150.f &&
		   !MotionMatching.DatabaseTags.Contains(AnimNames.PivotsTag);
}

bool UGASPAnimInstance::JustLanded_Light() const
{
	return FMath::Abs(CharacterInfo.Velocity.Z) < FMath::Abs(HeavyLandSpeedThreshold) && bLanded;
}

bool UGASPAnimInstance::JustLanded_Heavy() const
{
	return FMath::Abs(CharacterInfo.Velocity.Z) >= FMath::Abs(HeavyLandSpeedThreshold) && bLanded;
}

bool UGASPAnimInstance::JustTraversed() const
{
	return !IsSlotActive(AnimNames.AnimationSlotName) && GetCurveValue(AnimNames.MovingTraversalCurveName) > 0.f &&
		FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MotionMatching.FutureVelocity.GetSafeNormal(),
			CharacterInfo.Velocity.GetSafeNormal()))) <= CharacterInfo.MaxTurnAngle;
}

float UGASPAnimInstance::GetLandVelocity() const
{
	return CharacterInfo.Velocity.Z;
}

bool UGASPAnimInstance::PlayLand() const
{
	return MovementMode.IsOnGround() && PreviousMovementMode.IsInAir();
}

bool UGASPAnimInstance::PlayMovingLand() const
{
	return MovementMode.IsOnGround() && PreviousMovementMode.IsInAir() && FMath::Abs(GetTrajectoryTurnAngle()) <= 120.f;
}

float UGASPAnimInstance::GetTrajectoryTurnAngle() const
{
	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MotionMatching.FutureVelocity,
		CharacterInfo.Velocity)));
}

FVector2D UGASPAnimInstance::GetLeanAmount() const
{
	if (!CachedCharacter.IsValid())
		return FVector2D::ZeroVector;
	
	const FVector RelAccel = GetRelativeAcceleration();
	return FVector2D(RelAccel * FMath::GetMappedRangeValueClamped<float, float>({200.f, 500.f},
		{.5f, 1.f}, CharacterInfo.Speed));
}

FVector UGASPAnimInstance::GetRelativeAcceleration() const
{
	if (!CachedMovement.IsValid())
		return FVector::ZeroVector;

	const float Dot = FVector::DotProduct(CharacterInfo.Velocity, CharacterInfo.Acceleration);
	const float MaxAccelValue = (Dot > 0.f)
		? CachedMovement->GetMaxAcceleration()
		: CachedMovement->GetMaxBrakingDeceleration();

	const FVector ClampedAcceleration = CharacterInfo.Acceleration.GetClampedToMaxSize(MaxAccelValue) / MaxAccelValue;
	return CharacterInfo.ActorTransform.GetRotation().UnrotateVector(ClampedAcceleration);
}

void UGASPAnimInstance::RefreshMotionMatchingMovement(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!LocomotionTable)
		return;

	EAnimNodeReferenceConversionResult Result{};
	const FMotionMatchingAnimNodeReference Reference = UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node,
		Result);
	if (Result == EAnimNodeReferenceConversionResult::Failed)
		return;

	const TArray<UObject*> Objects{UChooserFunctionLibrary::EvaluateChooserMulti(this, LocomotionTable,
		UPoseSearchDatabase::StaticClass())};
	TArray<UPoseSearchDatabase*> Databases{};
	
	Algo::Transform(Objects, Databases, [](UObject *Object)
		{ return StaticCast<UPoseSearchDatabase*>(Object); });
	if (Databases.IsEmpty())
		return;
	
	UMotionMatchingAnimNodeLibrary::SetDatabasesToSearch(Reference, Databases, GetMatchingInterruptMode());
}

void UGASPAnimInstance::RefreshMatchingPostSelection(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result{};
	const FMotionMatchingAnimNodeReference Reference{UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node,
		Result)};
	if (Result == EAnimNodeReferenceConversionResult::Failed)
		return;

	FPoseSearchBlueprintResult OutResult{};
	bool bIsValidResult{};

	UMotionMatchingAnimNodeLibrary::GetMotionMatchingSearchResult(Reference, OutResult, bIsValidResult);
	MotionMatching.PoseSearchDatabase = OutResult.SelectedDatabase;
}

void UGASPAnimInstance::RefreshOffsetRoot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!OffsetRootBoneEnabled)
		return;

	const FTransform TargetTransform{ UAnimationWarpingLibrary::GetOffsetRootTransform(Node) };
	FRotator TargetRotation{ TargetTransform.Rotator() };
	TargetRotation.Yaw += 90.f;

	CharacterInfo.RootTransform = FTransform(TargetRotation, TargetTransform.GetLocation(),
		FVector::OneVector);
}

FQuat UGASPAnimInstance::GetDesiredFacing() const
{
	return Trajectory.GetSampleAtTime(.5f, false).Facing;
}

void UGASPAnimInstance::RefreshBlendStack(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	MotionMatching.AnimTime = UBlendStackAnimNodeLibrary::GetCurrentBlendStackAnimAssetTime(Node);
	MotionMatching.AnimAsset = UBlendStackAnimNodeLibrary::GetCurrentBlendStackAnimAsset(Node);

	const UAnimSequence *NewAnimSequence = Cast<UAnimSequence>(MotionMatching.AnimAsset);
	if (!NewAnimSequence)
		return;

	UAnimationWarpingLibrary::GetCurveValueFromAnimation(NewAnimSequence, AnimNames.EnableMotionWarpingCurveName,
		MotionMatching.AnimTime, MotionMatching.OrientationAlpha);
}

void UGASPAnimInstance::RefreshEssentialValues(const float DeltaSeconds)
{
	CharacterInfo.ActorTransform = CachedCharacter->GetActorTransform();

	if (!OffsetRootBoneEnabled)
	{
		CharacterInfo.RootTransform = CharacterInfo.ActorTransform;
	}

	// Refresh velocity variables
	
	CharacterInfo.Velocity = CachedCharacter->GetVelocity();
	CharacterInfo.Speed = CharacterInfo.Velocity.Size2D();
	CharacterInfo.HasVelocity = CharacterInfo.Speed > 5.f;

	// Calculate rate of change velocity
	CharacterInfo.Acceleration = (CharacterInfo.Velocity - PreviousCharacterInfo.Velocity) /
		FMath::Max(DeltaSeconds, .001f);

	if (CharacterInfo.HasVelocity)
	{
		MotionMatching.LastNonZeroVector = CharacterInfo.Velocity;
	}
}

bool UGASPAnimInstance::isEnabledAO() const
{
	return FMath::Abs(GetAOValue().X) <= 115.f && RotationMode.IsStrafe() &&
		   GetSlotMontageLocalWeight(AnimNames.AnimationSlotName) < .5f;
}

FVector2D UGASPAnimInstance::GetAOValue() const
{
	if (!CachedCharacter.IsValid())
		return FVector2D::ZeroVector;

	FRotator DeltaRot = ((CachedCharacter->IsLocallyControlled() ? CachedCharacter->GetControlRotation() :
		CachedCharacter->GetBaseAimRotation()) - CharacterInfo.RootTransform.Rotator()).GetNormalized();

	return FMath::Lerp({DeltaRot.Pitch, DeltaRot.Yaw}, FVector2D::ZeroVector, 
		GetCurveValue(AnimNames.DisableAOCurveName));
}
