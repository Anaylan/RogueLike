// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SendGameplayEvent.generated.h"
/**
 *
 */
UCLASS(DisplayName = "SendGameplayEvent")
class ROGUELIKE_API UAnimNotify_SendGameplayEvent : public UAnimNotify {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = AnimNotify)
    FGameplayTag GameplayTag;

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;
};
