// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/MeleeWeapon.h"
#include "Components/BoxComponent.h"


// Sets default values
AMeleeWeapon::AMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(GetMesh());
}

FVector AMeleeWeapon::GetStartPoint()
{
	return CollisionComponent->GetComponentLocation();
}

FVector AMeleeWeapon::GetEndPoint()
{
	return CollisionComponent->GetComponentLocation();
}
