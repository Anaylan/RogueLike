// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Helpers/CommonHelper.h"

AActor* FCommonHelper::FindClosestActor(TArray<AActor*> Actors, const AActor* FromActor)
{
	if (Actors.IsEmpty())
	{
		return nullptr;
	}

	AActor* NearestActor = *Algo::MinElementBy(Actors, [FromActor](const AActor* Actor)
	{
		return Actor->GetDistanceTo(FromActor);
	});
	return NearestActor;
}
