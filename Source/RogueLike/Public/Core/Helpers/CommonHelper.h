// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class ROGUELIKE_API FCommonHelper
{
public:
	static AActor* FindClosestActor(TArray<AActor*> Actors, const AActor* FromActor);
};
