// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent, NAME_None);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseWeapon::AttachTo(AActor* NewOwner)
{
	SetOwner(NewOwner);
}

void ABaseWeapon::Detach()
{
	SetOwner(nullptr);
}
