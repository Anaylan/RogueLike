// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/BaseCharacter.h"
#include "Actors/Weapons/BaseWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
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

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Attributes = CreateDefaultSubobject<UHopperAttributeSet>(TEXT("Attributes"));
}

void ABaseCharacter::OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			WeaponSocket);
		CurrentWeapon->AttachTo(this);
		if (OverlayState != CurrentWeapon->GetOverlayState())
		{
			SetOverlayState(CurrentWeapon->GetOverlayState());
		}
	}
	else
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Detach();
		CurrentWeapon = nullptr;
		SetOverlayState(EOverlayState::Default);
	}

	if (OldPrimaryWeapon)
	{
		OldPrimaryWeapon->SetOwner(nullptr);
		OldPrimaryWeapon = nullptr;
	}
}

void ABaseCharacter::SpawnWeapons()
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

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_SkipOwner;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, OverlayState, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, SpawnedWeapons, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentWeapon, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponIndex, Params);
}

bool ABaseCharacter::HasWeapon()
{
	return IsValid(CurrentWeapon);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapons();

}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseCharacter::AddStartupGameplayAbilities()
{
	if (!ensure(AbilitySystemComponent))
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (auto& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				StartupAbility.Value, 1, StaticCast<int32>(StartupAbility.Key), this));
		}

		// Now apply passives
		for (const auto& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// called in SetupPlayerInputComponent() for redundancy
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			FTopLevelAssetPath(GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>(
				"/Script/Prototype.EAbilityInputID"))),
			StaticCast<int32>(EAbilityInputID::Confirm),
			StaticCast<int32>(EAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

bool ABaseCharacter::CanAttack() const
{
	if (!IsValid(AbilitySystemComponent))
		return false;
	
	return true;
}

void ABaseCharacter::SetOverlayState(const EOverlayState NewOverlayState, const bool bForce)
{
	if (NewOverlayState != OverlayState || bForce)
	{
		OverlayState = NewOverlayState;

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetOverlayState(NewOverlayState);
		}
	}
}

void ABaseCharacter::Server_SetOverlayState_Implementation(const EOverlayState NewOverlayState)
{
	OverlayState = NewOverlayState;
}

void ABaseCharacter::TryUseAbility(EAbilityInputID AbilityInputID)
{	
	if (!AbilitySystemComponent) return;
	if (!GameplayAbilities.Contains(AbilityInputID) || 
		!IsValid(GameplayAbilities[AbilityInputID].GetDefaultObject())) return;
	
	AbilitySystemComponent->TryActivateAbilityByClass(GameplayAbilities[AbilityInputID]);
}