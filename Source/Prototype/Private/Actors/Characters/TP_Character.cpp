// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/TP_Character.h"
#include "Actors/Weapons/BaseWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"


ATP_Character::ATP_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
}

void ATP_Character::EquipAction()
{
}

void ATP_Character::AttackAction()
{
}

void ATP_Character::OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			WeaponSocket);
		CurrentWeapon->AttachTo(this);
		SetOverlayState(CurrentWeapon->GetOverlayState());
		//CurrentWeapon->Equip(this);
		//SetGlobalStance(EPawnGlobalStance::Armed);
	}
	else
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Detach();
		CurrentWeapon = nullptr;
		//SetGlobalStance(EPawnGlobalStance::Unarmed);
	}

	if (OldPrimaryWeapon)
	{
		OldPrimaryWeapon->SetOwner(nullptr);
		OldPrimaryWeapon = nullptr;
	}
}

void ATP_Character::SpawnWeapons()
{
	if (HasAuthority())
	{
		for (auto& WeaponClass : Weapons)
		{
			if (!IsValid(WeaponClass)) continue;

			FActorSpawnParameters Params;
			Params.Owner = this;

			ABaseWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, Params);
			const int32 Index = SpawnedWeapons.Add(SpawnedWeapon);
			SpawnedWeapon->AttachTo(this);

			if (Index == WeaponIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

void ATP_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, SpawnedWeapons, ELifetimeCondition::COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeapon, ELifetimeCondition::COND_None);
}

bool ATP_Character::HasWeapon()
{
	return false;
}

void ATP_Character::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapons();
}
