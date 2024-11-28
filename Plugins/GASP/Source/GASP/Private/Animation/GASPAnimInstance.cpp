// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/B_AnimInstance.h"
#include "Actors/B_Character.h"
#include "Components/B_CharacterMovementComponent.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "Utils/AnimationUtils.h"
#include "PoseSearch/MotionMatchingAnimNodeLibrary.h"
#include "ChooserFunctionLibrary.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "AnimationWarpingLibrary.h"
#include "BlendStack/BlendStackAnimNodeLibrary.h"
#include "BoneControllers/AnimNode_FootPlacement.h"


void UB_AnimInstance::OnLanded(const FHitResult& HitResult)
{
	bLanded = true;
	GetWorld()->GetTimerManager().SetTimer(LandedTimer, [this]() { bLanded = false; }, .2f,
		false);
}

EPoseSearchInterruptMode UB_AnimInstance::GetMatchingInterruptMode() const
{
	if (MovementMode != PreviousMovementMode || (MovementMode.isOnGround() && 
		(MovementState != PreviousMovementState || Gait != PreviousGait || 
			RotationMode != PreviousRotationMode)))
	{
		return EPoseSearchInterruptMode::InterruptOnDatabaseChange;
	}
	return EPoseSearchInterruptMode::DoNotInterrupt;
}

EOffsetRootBoneMode UB_AnimInstance::GetOffsetRootRotationMode() const
{
	return IsSlotActive(AnimNames.AnimationSlotName) ? EOffsetRootBoneMode::Release : EOffsetRootBoneMode::Accumulate;
}

EOffsetRootBoneMode UB_AnimInstance::GetOffsetRootTranslationMode() const
{
	if (IsSlotActive(AnimNames.AnimationSlotName)) return EOffsetRootBoneMode::Release;
	if (MovementMode.isOnGround())
	{
		return isMoving() ? EOffsetRootBoneMode::Interpolate : EOffsetRootBoneMode::Release;
	}
	if (MovementMode.isInAir())
	{
		return EOffsetRootBoneMode::Release;
	}
	return EOffsetRootBoneMode();
}

float UB_AnimInstance::GetOffsetRootTranslationHalfLife() const
{
	if (MovementState.isIdle()) return .1f;
	if (MovementState.isMoving()) return .2f;
	return 0.f;
}

EOrientationWarpingSpace UB_AnimInstance::GetOrientationWarpingSpace() const
{
	return OffsetRootBoneEnabled ? EOrientationWarpingSpace::RootBoneTransform : EOrientationWarpingSpace::ComponentTransform;
} 

bool UB_AnimInstance::IsEnableSteering() const
{
	return isMoving() || MovementMode.isInAir();
}

void UB_AnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	CachedCharacter = StaticCast<AB_Character*>(TryGetPawnOwner());
	if (!CachedCharacter.IsValid()) return;
	CachedMovement = CachedCharacter->FindComponentByClass<UB_CharacterMovementComponent>();
	if (!CachedMovement.IsValid()) return;

	CachedCharacter->LandedDelegate.AddDynamic(this, &UB_AnimInstance::OnLanded);
}

void UB_AnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!CachedCharacter.IsValid()) return;
	if (!CachedMovement.IsValid()) return;

	//FMovementAnimInstanceProxy Proxy{ GetProxyOnGameThread<FMovementAnimInstanceProxy>() };
	PreviousCharacterInfo = CharacterInfo;
	GenerateTrajectory(DeltaSeconds);
	UpdateCharacterInfo();
	UpdateEssentialValues(DeltaSeconds);
	UpdateMotionMatchingInfo();
}

void UB_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedCharacter.IsValid()) return;

	Gait = CachedCharacter->Gait;
	RotationMode = CachedCharacter->RotationMode;
	MovementState = CachedCharacter->MovementState;
	MovementMode = CachedCharacter->MovementMode;

	if (!CachedMovement.IsValid()) return;
}

void UB_AnimInstance::BeginDestroy()
{
	Super::BeginDestroy();
}

void UB_AnimInstance::UpdateCharacterInfo()
{
	// Refresh velocity last frame
	PreviousCharacterInfo.Velocity = CharacterInfo.Velocity;

	CharacterInfo.ActorTransform = CachedCharacter->GetActorTransform();
	CharacterInfo.Velocity = CachedCharacter->GetVelocity();

	CharacterInfo.Direction = UAnimationUtils::CalculateDirection(CharacterInfo.Velocity, 
		CharacterInfo.ActorTransform);

	CharacterInfo.Speed = CharacterInfo.Velocity.Size2D();
}

void UB_AnimInstance::UpdateMotionMatchingInfo()
{
	if (CharacterInfo.Speed > 0.f) MotionMatching.LastNonZeroVector = CharacterInfo.Velocity;
	if (MotionMatching.PoseSearchDatabase.IsValid()) MotionMatching.DatabaseTags = MotionMatching.PoseSearchDatabase->Tags;

	MotionMatching.Alpha = MotionMatching.DatabaseTags.Contains(AnimNames.TurnInPlaceTag) ? .5f : .25f;
}

void UB_AnimInstance::GenerateTrajectory(float DeltaSeconds)
{
	const auto& TrajectoryData{ CharacterInfo.Speed > 0.f ? TrajectoryGenerationData_Moving : TrajectoryGenerationData_Idle };
	FPoseSearchQueryTrajectory OutTrajectory;
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory(this, TrajectoryData, DeltaSeconds,
		Trajectory, MotionMatching.PreviousDesiredYawRotation, OutTrajectory,
		-1.f, 30, .1f, 15);

	const TArray<AActor*> IgnoredActors{ };
	UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions(CachedCharacter.Get(), this,
		OutTrajectory, true, .01f, Trajectory, CollisionResults,
		TraceTypeQuery_MAX, false, IgnoredActors, EDrawDebugTrace::None, true, 
150.f);

	UPoseSearchTrajectoryLibrary::GetTrajectoryVelocity(Trajectory, .4f, .5f, MotionMatching.FutureVelocity, false);
}

float UB_AnimInstance::GetMatchingBlendTime() const
{
	if (MovementMode.isInAir())
	{
		return GetLandVelocity() > 100.f ? .15f : .5f;
	}
	
	return PreviousMovementMode.isOnGround() ? .5f : .2f;
}

FFootPlacementPlantSettings UB_AnimInstance::GetPlantSettings() const
{
	return MotionMatching.DatabaseTags.Contains(AnimNames.StopsTag) ? PlantSettings_Stops : PlantSettings_Default;
}

FFootPlacementInterpolationSettings UB_AnimInstance::GetPlantInterpolationSettings() const
{
	return MotionMatching.DatabaseTags.Contains(AnimNames.StopsTag) ? InterpolationSettings_Stops : InterpolationSettings_Default;
}

float UB_AnimInstance::Get_MatchingNotifyRecencyTimeOut()
{
	static constexpr float RecencyTimeOuts[] = { .2f, .2f, .16f };
	// Take a closer look, maybe FGait should be a EGait
	return Gait >= EGait::Walk && Gait <= EGait::Sprint ? RecencyTimeOuts[StaticCast<int32>(
		StaticCast<EGait>(Gait))] : .2f;
}

bool UB_AnimInstance::isStarting() const
{
	return (MotionMatching.FutureVelocity.SizeSquared2D() > FMath::Square(CharacterInfo.Velocity.Size2D() + 100.f)) &&
		!MotionMatching.DatabaseTags.Contains("Pivots") &&
		MovementState.isMoving() &&
		CachedMovement.IsValid();
}

bool UB_AnimInstance::isMoving() const
{
	return CharacterInfo.Velocity != FVector::ZeroVector &&
		MotionMatching.FutureVelocity != FVector::ZeroVector;
}

bool UB_AnimInstance::isPivoting() const
{
	if (MotionMatching.FutureVelocity.IsNearlyZero() || CharacterInfo.Velocity.IsNearlyZero())
	{
		return false;
	}
	
	float DotProduct = FVector::DotProduct(MotionMatching.FutureVelocity.GetSafeNormal(),
		CharacterInfo.Velocity.GetSafeNormal());
	float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	
	return AngleDegrees >= (RotationMode.isStrafe() ? 30.f : 45.f) && isMoving();
}

bool UB_AnimInstance::ShouldTurnInPlace() const
{
	FVector ActorForward = CharacterInfo.ActorTransform.GetRotation().GetForwardVector();
	FVector RootForward = CharacterInfo.RootTransform.GetRotation().GetForwardVector();

	float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ActorForward,
		RootForward)));
	return AngleDegrees >= CharacterInfo.MaxTurnAngle && MovementState.isIdle();
}

bool UB_AnimInstance::ShouldSpin() const
{
	const FVector ActorForward = CharacterInfo.ActorTransform.GetRotation().GetForwardVector();
	const FVector RootForward = CharacterInfo.RootTransform.GetRotation().GetForwardVector();

	float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ActorForward,
		RootForward)));
	return AngleDegrees >= 130.f && CharacterInfo.Speed >= 150.f &&
		!MotionMatching.DatabaseTags.Contains("Pivots");
}

bool UB_AnimInstance::JustLanded_Light() const
{
	return FMath::Abs(CharacterInfo.Velocity.Z) < FMath::Abs(HeavyLandSpeedThreshold) && bLanded;
}

bool UB_AnimInstance::JustLanded_Heavy() const
{
	return FMath::Abs(CharacterInfo.Velocity.Z) >= FMath::Abs(HeavyLandSpeedThreshold) && bLanded;
}

bool UB_AnimInstance::JustTraversal() const
{
	return !IsSlotActive(AnimNames.AnimationSlotName) && GetCurveValue(AnimNames.MovingTraversalCurveName) > 0.f &&
		FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MotionMatching.FutureVelocity.GetSafeNormal(),
			CharacterInfo.Velocity.GetSafeNormal()))) <= CharacterInfo.MaxTurnAngle;
}

float UB_AnimInstance::GetLandVelocity() const
{
	return CharacterInfo.Velocity.Z;
}

bool UB_AnimInstance::PlayLand() const
{
	return MovementMode.isOnGround() && PreviousMovementMode.isInAir();
}

bool UB_AnimInstance::PlayMovingLand() const
{
	return MovementMode.isOnGround() && PreviousMovementMode.isInAir() && FMath::Abs(GetTrajectoryTurnAngle()) <= 120.f;
}

float UB_AnimInstance::GetTrajectoryTurnAngle() const
{
	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MotionMatching.FutureVelocity,
		CharacterInfo.Velocity)));
}

float UB_AnimInstance::GetFloatLeanAmount() const
{
	if (!CachedCharacter.IsValid()) return .0f;
	const FVector RelAccel = GetRelativeAcceleration();

	return FMath::GetMappedRangeValueClamped<float, float>({ 200.f, 500.f }, 
	{ .5f, 1.f }, CharacterInfo.Speed) * RelAccel.Y;
}

FVector UB_AnimInstance::GetRelativeAcceleration() const
{
	if (!CachedMovement.IsValid()) return FVector::ZeroVector;
	
	const float Dot = FVector::DotProduct(CharacterInfo.Velocity,
		CharacterInfo.Acceleration);
	const float MaxAccelValue = (Dot > 0.f)
		? CachedMovement->GetMaxAcceleration()
		: CachedMovement->GetMaxBrakingDeceleration();
	
	const FVector ClampedAcceleration = CharacterInfo.VelocityAcceleration.GetClampedToMaxSize(MaxAccelValue) / MaxAccelValue;
	return CharacterInfo.ActorTransform.GetRotation().UnrotateVector(ClampedAcceleration);
}

float UB_AnimInstance::GetYawOffset() const
{
	if (!CachedCharacter.IsValid()) return 0.f;

	const float Angle = (CachedCharacter->GetControlRotation() - CharacterInfo.RootTransform.Rotator()).Yaw;
	return FMath::ClampAngle(FMath::Fmod(Angle, 360.f), -90.f, 90.f);
}

float UB_AnimInstance::GetPitchOffset() const
{
	if (!CachedCharacter.IsValid()) return 0.f;
	return FRotator::NormalizeAxis(CachedCharacter->GetActorRotation().Pitch);
}

void UB_AnimInstance::UpdateMotionMatchingMovement(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!LocomotionTable) return;

	EAnimNodeReferenceConversionResult Result{};
	const FMotionMatchingAnimNodeReference Reference = UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node,
		Result);
	if (Result == EAnimNodeReferenceConversionResult::Failed) return;

	UPoseSearchDatabase* Database = Cast<UPoseSearchDatabase>(UChooserFunctionLibrary::EvaluateChooser(
		this, LocomotionTable, UPoseSearchDatabase::StaticClass()));
	if (!Database) return;
	UMotionMatchingAnimNodeLibrary::SetDatabaseToSearch(Reference, Database,
		GetMatchingInterruptMode());
}

void UB_AnimInstance::UpdateMatchingPostSelection(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result{};
	FMotionMatchingAnimNodeReference Reference = UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node,
		Result);
	if (Result == EAnimNodeReferenceConversionResult::Failed) return;

	FPoseSearchBlueprintResult OutResult;
	bool bIsValidResult{};

	UMotionMatchingAnimNodeLibrary::GetMotionMatchingSearchResult(Reference, OutResult, bIsValidResult);
	MotionMatching.PoseSearchDatabase = OutResult.SelectedDatabase;
}

void UB_AnimInstance::UpdateOffsetRoot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!OffsetRootBoneEnabled)
	{
		CharacterInfo.RootTransform = CharacterInfo.ActorTransform;
		return;
	}
	const FTransform TargetTransform = UAnimationWarpingLibrary::GetOffsetRootTransform(Node);
	FRotator TargetRotation = TargetTransform.Rotator();
	TargetRotation.Yaw += 90.f;
	CharacterInfo.RootTransform = FTransform(TargetRotation, TargetTransform.GetLocation(),
		FVector::OneVector);
}

FQuat UB_AnimInstance::GetDesiredFacing() const
{
	return Trajectory.GetSampleAtTime(.5f, false).Facing;
}

void UB_AnimInstance::UpdateBlendStack(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	MotionMatching.AnimTime = UBlendStackAnimNodeLibrary::GetCurrentBlendStackAnimAssetTime(Node);
	MotionMatching.AnimAsset = UBlendStackAnimNodeLibrary::GetCurrentBlendStackAnimAsset(Node);
	const UAnimSequence* NewAnimSequence = Cast<UAnimSequence>(MotionMatching.AnimAsset);
	if (!NewAnimSequence) return;
	UAnimationWarpingLibrary::GetCurveValueFromAnimation(NewAnimSequence, AnimNames.EnableMotionWarpingCurveName,
		MotionMatching.AnimTime, MotionMatching.OrientationAlpha);
}

void UB_AnimInstance::UpdateEssentialValues(float DeltaSeconds)
{
	CharacterInfo.ActorTransform = CachedCharacter->GetActorTransform();

	// Get console variables
	const auto& OffsetRootEnabled = IConsoleManager::Get().FindConsoleVariable(
		TEXT("a.animnode.offsetrootbone.enable"));
	const auto& MMLod = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.MMDatabaseLOD"));
	if (OffsetRootEnabled)
	{
		OffsetRootBoneEnabled = OffsetRootEnabled->GetBool();
	}
	if (MMLod)
	{
		MMDatabaseLOD = MMLod->GetInt();
	}
	
	if (OffsetRootBoneEnabled) {
		CharacterInfo.RootTransform = CharacterInfo.ActorTransform;
	}

	// Refresh velocity variables
	CharacterInfo.Velocity = CachedCharacter->GetVelocity();
	CharacterInfo.Speed = CharacterInfo.Velocity.Size2D();
	CharacterInfo.HasVelocity = CharacterInfo.Speed > 5.f;
	
	// Refresh acceleration variables
	CharacterInfo.Acceleration =  CachedMovement->GetCurrentAcceleration();
	
	CharacterInfo.AccelerationAmount = CharacterInfo.Acceleration.Size() / CachedMovement->GetMaxAcceleration();
	CharacterInfo.HasAcceleration = CharacterInfo.AccelerationAmount > 0.f;
	
	// Calculate rate of change velocity
	CharacterInfo.VelocityAcceleration = (CharacterInfo.Velocity - PreviousCharacterInfo.Velocity) / FMath::Max(DeltaSeconds, .001f);

	if (CharacterInfo.HasVelocity) {
		MotionMatching.LastNonZeroVector = CharacterInfo.Velocity;
	}

	if (MMDatabaseLOD && MotionMatching.PoseSearchDatabase.IsValid()) {
		MotionMatching.DatabaseTags = MotionMatching.PoseSearchDatabase->Tags;
	}
}

bool UB_AnimInstance::isEnabledAO()
{
	return FMath::Abs(Get_AOValue().X) <= 115.f && RotationMode.isStrafe() && 
		GetSlotMontageLocalWeight(AnimNames.AnimationSlotName) < .5f;
}

FVector2D UB_AnimInstance::Get_AOValue() const
{
	if (!CachedCharacter.IsValid() || !CachedCharacter->GetController()) return FVector2D::ZeroVector;
	const FRotator ControlRotation{CachedCharacter->IsLocallyControlled() ? CachedCharacter->GetControlRotation() :
		CachedCharacter->GetBaseAimRotation() };
	
	FRotator DeltaRot = (ControlRotation - CharacterInfo.RootTransform.Rotator()).GetNormalized();
	return FMath::Lerp({DeltaRot.Pitch, DeltaRot.Yaw }, FVector2D::ZeroVector,
		GetCurveValue(AnimNames.DisableAOCurveName));
}
