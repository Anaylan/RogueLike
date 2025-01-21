// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Core/Types/PawnTypes.h"
#include "AnimNotifyState_MeleeTrace.generated.h"

class UAbilitySystemComponent;
class ABaseWeapon;
/**
 *
 */
UCLASS(DisplayName = "MeleeTrace")
class ROGUELIKE_API UAnimNotifyState_MeleeTrace : public UAnimNotifyState {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
    TSet<TObjectPtr<AActor>> OverlappedActors;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseUnarmedBone", EditConditionHides))
    FBoneReference BoneReference;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseWeapon", EditConditionHides))
    EWeaponSlot WeaponSlot;

    TArray<AActor*> IgnoredActors;
    FCollisionQueryParams QueryParams;

    TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    TWeakObjectPtr<ABaseWeapon> Weapon;
    TWeakObjectPtr<UWorld> WorldContext;

private:
    UPROPERTY()
    uint8 bUseUnarmedBone : 1 { true };
    UPROPERTY()
    uint8 bUseWeapon : 1 { true };

public:
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
        const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
        const FAnimNotifyEventReference& EventReference) override;

    void PerformSweep(AActor* Owner, const FVector& StartLocation, const FVector& EndLocation);

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
    {
        Super::PostEditChangeProperty(PropertyChangedEvent);

        const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

        if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, WeaponSlot)) {
            bUseUnarmedBone = WeaponSlot == EWeaponSlot::None;
        }

        bUseWeapon = BoneReference.BoneName.IsNone();
    }
#endif
};
