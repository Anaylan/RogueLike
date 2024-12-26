
#include "Utils/AnimationUtils.h"

float UAnimationUtils::CalculateDirection(const FVector& Velocity, const FRotator& ActorRotation)
{
	if (Velocity.IsNearlyZero() || ActorRotation.IsNearlyZero())
	{
		return 0.f;
	}

	const FVector NormalizedVelocity{ Velocity.GetSafeNormal2D() };
	
	const float YawRad = FMath::DegreesToRadians(ActorRotation.Yaw);

	// Get Forward and Right vectors from ActorRotation
	const FVector ForwardVector{ FVector(FMath::Cos(YawRad), FMath::Sin(YawRad), 0.f) };
	const FVector RightVector{ FVector(-ForwardVector.Y, ForwardVector.X, 0.f) };
	
	// Projection speed vector to direction vector
	const float ForwardDot = FVector::DotProduct(ForwardVector, NormalizedVelocity);
	const float RightDot = FVector::DotProduct(RightVector, NormalizedVelocity);

	// Get angle in degrees
	return FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
}
