// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	APawn* Owner = static_cast<APawn*>(MeshComp->GetOwner());
	if (!IsValid(Owner))
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent{};
	if (const IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Owner))
	{
		AbilitySystemComponent = AbilityInterface->GetAbilitySystemComponent();
	}
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	const FGameplayEventData Payload{};
	AbilitySystemComponent->HandleGameplayEvent(GameplayTag, &Payload);
}
