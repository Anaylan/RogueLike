// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_BaseSweepTrace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Actors/Weapons/MeleeWeapon.h"

void UAnimNotifyState_BaseSweepTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	OverlappedActors.Empty();
	IgnoredActors.Empty();
	QueryParams.ClearIgnoredSourceObjects();

	AbilitySystemComponent.Reset();
}

void UAnimNotifyState_BaseSweepTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration,
                                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	OverlappedActors.Empty();

	if (!IsValid(MeshComp))
	{
		return;
	}

	Owner = Cast<APawn>(MeshComp->GetOwner());
	if (!Owner.IsValid())
	{
		return;
	}

	WorldContext = Owner->GetWorld();

	Owner->GetAllChildActors(IgnoredActors);

	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(Owner.Get());
	QueryParams.AddIgnoredActors(IgnoredActors);

	if (const IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Owner))
	{
		AbilitySystemComponent = AbilityInterface->GetAbilitySystemComponent();
	}

#if ALLOW_CONSOLE
	const auto& DebugEnabled = IConsoleManager::Get().FindConsoleVariable(
		TEXT("DDCVar.DebugWeaponHitTraces.Enable"));
	if (DebugEnabled)
	{
		bDebugEnabled = DebugEnabled->GetBool();
	}
#endif
}

void UAnimNotifyState_BaseSweepTrace::PerformSweep(const FVector& StartLocation,
                                                   const FVector& EndLocation,
                                                   const FVector& ShapeExtent,
                                                   const FQuat& Rotation)
{
	if (!WorldContext.IsValid() || !WorldContext->IsGameWorld())
	{
		UE_LOG(LogTemp, Display, TEXT("World is not valid"))
		return;
	}

	TArray<FHitResult> OutHits;


	if (WorldContext->SweepMultiByChannel(OutHits, StartLocation, EndLocation, Rotation, CollisionChannel,
	                                      FCollisionShape::MakeBox(ShapeExtent), QueryParams))
	{
		for (auto& OutHit : OutHits)
		{
			if (!IsValid(OutHit.GetActor()) || OverlappedActors.Contains(OutHit.GetActor()))
			{
				continue;
			}

#if ALLOW_CONSOLE
			if (bDebugEnabled)
			{
				DrawDebugBox(WorldContext.Get(), OutHit.ImpactPoint, FVector(10.f),
				             FColor::Green, false, 10.f);
			}
#endif
			OverlappedActors.Add(OutHit.GetActor());
			FGameplayEventData Payload{};
			Payload.Instigator = Owner.Get();
			Payload.Target = OutHit.GetActor();
			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OutHit.GetActor());

			AbilitySystemComponent->HandleGameplayEvent(HitTag, &Payload);
		}
	}
}
