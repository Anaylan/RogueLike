// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotifyState_SweepMultiByChannel.h"
#include "CollisionShape.h"
#include "AbilitySystemComponent.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Actors/Weapons/BaseWeapon.h"


void UAnimNotifyState_SweepMultiByChannel::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp))
		return;
	
	APawn* Owner = StaticCast<APawn*>(MeshComp->GetOwner());
	if (!IsValid(Owner)) return;

	UAbilitySystemComponent* AbilitySystemComponent = Owner->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(AbilitySystemComponent)) return;
	
	USkeletalMeshComponent* WeaponMesh{ nullptr };
	if (IWeaponInterface* Interface = Cast<IWeaponInterface>(Owner))
	{
		WeaponMesh = Interface->GetCurrentWeapon()->GetMesh();
	}
	if (!IsValid(WeaponMesh))
		return;

	TArray<AActor*> IgnoredActors;
	Owner->GetAllChildActors(IgnoredActors);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActors(IgnoredActors);

	const FTransform MeshTransform = WeaponMesh->GetComponentTransform();

	const FBoxSphereBounds Bounds = WeaponMesh->Bounds;
	const FVector MeshExtent = Bounds.BoxExtent;

	const FVector LocalStartOffset = FVector(0.f, 0.0f, 0.f);
	const FVector LocalEndOffset = FVector(0.f, 0.0f, MeshExtent.X * 2);

	const FVector StartLocation = MeshTransform.TransformPosition(LocalStartOffset);
	const FVector EndLocation = MeshTransform.TransformPosition(LocalEndOffset);
	
	SweepMultiByChannel(Owner, StartLocation, EndLocation, AbilitySystemComponent, QueryParams);
}

void UAnimNotifyState_SweepMultiByChannel::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	OverlappedActors.Empty();
}

void UAnimNotifyState_SweepMultiByChannel::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	OverlappedActors.Empty();
}

void UAnimNotifyState_SweepMultiByChannel::SweepMultiByChannel(APawn* Owner, const FVector& StartLocation,
	const FVector& EndLocation, UAbilitySystemComponent* AbilitySystemComponent, const FCollisionQueryParams& CollisionQueryParams)
{
	const UWorld* World = Owner->GetWorld();
	if (!IsValid(World) || !World->IsGameWorld())
		return;
	
	TArray<FHitResult> Hits;
	
	if (World->SweepMultiByChannel(Hits, StartLocation, EndLocation, FQuat::Identity, ECC_GameTraceChannel18,
		FCollisionShape::MakeBox(FVector(10.f)), CollisionQueryParams))
	{
		for (auto& Hit : Hits)
		{
			if (!IsValid(Hit.GetActor()) || OverlappedActors.Contains(Hit.GetActor()))
				continue;
			
			OverlappedActors.Add(Hit.GetActor());
			
			FGameplayEventData Payload{ };
			Payload.Instigator = Owner;
			Payload.Target = Hit.GetActor();
			
			AbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(FName("Event.Hit")), &Payload);
		}
	}
}

