// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemComponent.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
		return;
	
	const APawn* Owner = StaticCast<APawn*>(MeshComp->GetOwner());
	if (!IsValid(Owner)) return;

	UAbilitySystemComponent* AbilitySystemComponent = Owner->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(AbilitySystemComponent)) return;

	const FGameplayEventData Payload{};
	AbilitySystemComponent->HandleGameplayEvent(GameplayTag, &Payload);
}
