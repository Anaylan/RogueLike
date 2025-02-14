// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotifyState_WeaponMeleeTrace.h"
#include "Actors/Weapons/MeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Core/Interfaces/WeaponInterface.h"

void UAnimNotifyState_WeaponMeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration,
                                                    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(Owner.Get()))
	{
		Weapon = Cast<AMeleeWeapon>(WeaponInterface->GetWeaponInSlot(WeaponSlot));
	}
	TArray<AActor*> WeaponChildActors;
	Weapon->GetAllChildActors(WeaponChildActors);

	QueryParams.AddIgnoredActor(Weapon.Get());
	QueryParams.AddIgnoredActors(WeaponChildActors);
}

void UAnimNotifyState_WeaponMeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	Weapon.Reset();
}

void UAnimNotifyState_WeaponMeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float FrameDeltaTime,
                                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!Weapon.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon is not valid"))
		return;
	}

	const FVector StartLocation{Weapon->GetStartPoint()};
	const FVector EndLocation{Weapon->GetEndPoint()};

	PerformSweep(StartLocation, EndLocation, Weapon->GetCollisionComponent()->GetScaledBoxExtent(),
	             Weapon->GetActorRotation().Quaternion());
}
