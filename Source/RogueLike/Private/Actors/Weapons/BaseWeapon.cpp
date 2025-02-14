// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Weapons/BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent, NAME_None);

	Attributes = CreateDefaultSubobject<UWeaponAttributeSet>(TEXT("Attributes"));
}
