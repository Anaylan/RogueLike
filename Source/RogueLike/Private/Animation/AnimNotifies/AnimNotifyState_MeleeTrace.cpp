// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_MeleeTrace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Core/Interfaces/WeaponInterface.h"
#include "Actors/Weapons/BaseWeapon.h"

void UAnimNotifyState_MeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (BoneReference.BoneName == NAME_None && WeaponSlot == EWeaponSlot::None)
        return;

    if (!IsValid(MeshComp))
        return;

    APawn* Owner = StaticCast<APawn*>(MeshComp->GetOwner());
    if (!IsValid(Owner))
        return;

    FVector StartLocation = Weapon.IsValid() ? Weapon->GetStartPoint() : MeshComp->GetBoneLocation(BoneReference.BoneName);
    FVector EndLocation = Weapon.IsValid() ? Weapon->GetEndPoint() : MeshComp->GetBoneLocation(BoneReference.BoneName);

    PerformSweep(Owner, StartLocation, EndLocation);
}

void UAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    OverlappedActors.Empty();
    IgnoredActors.Empty();
    QueryParams.ClearIgnoredSourceObjects();

    AbilitySystemComponent.Reset();
    Weapon.Reset();
}

void UAnimNotifyState_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    OverlappedActors.Empty();

    if (BoneReference.BoneName == NAME_None && WeaponSlot == EWeaponSlot::None)
        return;

    if (!IsValid(MeshComp))
        return;

    APawn* Owner = StaticCast<APawn*>(MeshComp->GetOwner());
    if (!IsValid(Owner))
        return;

    WorldContext = Owner->GetWorld();

    Owner->GetAllChildActors(IgnoredActors);

    QueryParams.bTraceComplex = true;
    QueryParams.AddIgnoredActor(Owner);
    QueryParams.AddIgnoredActors(IgnoredActors);

    if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Owner))
        AbilitySystemComponent = AbilityInterface->GetAbilitySystemComponent();

    if (IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(Owner))
        Weapon = WeaponInterface->GetCurrentWeapon(WeaponSlot);
}

void UAnimNotifyState_MeleeTrace::PerformSweep(AActor* Owner, const FVector& StartLocation, const FVector& EndLocation)
{
    if (!WorldContext.IsValid() || !WorldContext->IsGameWorld())
        return;

    TArray<FHitResult> Hits;

    if (WorldContext->SweepMultiByChannel(Hits, StartLocation, EndLocation, FQuat::Identity, ECC_GameTraceChannel18,
            FCollisionShape::MakeBox(FVector(10.f)), QueryParams)) {
        for (auto& Hit : Hits) {

            if (!IsValid(Hit.GetActor()) || OverlappedActors.Contains(Hit.GetActor()))
                continue;

            OverlappedActors.Add(Hit.GetActor());
            FGameplayEventData Payload {};
            Payload.Instigator = Owner;
            Payload.Target = Hit.GetActor();
            Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor());

            AbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(FName("Event.Hit")), &Payload);
        }
    }
}