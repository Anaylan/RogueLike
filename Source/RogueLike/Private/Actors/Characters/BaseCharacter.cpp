// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Characters/BaseCharacter.h"
#include "Actors/Weapons/BaseWeapon.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Abilities/Attributes/PawnAttributeSet.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bReplicates = true;

    CurrentWeapon = CreateDefaultSubobject<ABaseWeapon>("WeaponComponent");
}

void ABaseCharacter::OnRep_CurrentWeapon(ABaseWeapon* OldCurrentWeapon)
{
    if (CurrentWeapon) {
        USkeletalMeshComponent* MeshComponent { GetMesh() };
        const FName AttachSocket = CurrentWeapon->GetAttachSocket();
        CurrentWeapon->SetActorTransform(MeshComponent->GetSocketTransform(AttachSocket), false, nullptr,
            ETeleportType::TeleportPhysics);

        CurrentWeapon->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale,
            AttachSocket);
        CurrentWeapon->SetOwner(this);
        if (OverlayState != CurrentWeapon->GetOverlayState()) {
            SetOverlayState(CurrentWeapon->GetOverlayState());
        }
        GiveGameplayEffects(CurrentWeapon->GetGameplayEffects());
    } else {
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentWeapon->SetOwner(nullptr);
        CurrentWeapon->SetVisibility(false);
        CurrentWeapon = nullptr;

        SetOverlayState(EOverlayState::Default);
    }

    if (OldCurrentWeapon) {
        RemoveGameplayEffects(OldCurrentWeapon->GetGameplayEffects());
        OldCurrentWeapon->SetOwner(nullptr);
        OldCurrentWeapon = nullptr;
    }
}

void ABaseCharacter::SpawnWeapons()
{
    if (HasAuthority()) {
        for (auto& WeaponClass : Weapons) {
            if (!IsValid(WeaponClass))
                continue;

            FActorSpawnParameters Params;
            Params.Owner = this;

            ABaseWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, Params);
            const int32 Index = SpawnedWeapons.Add(SpawnedWeapon);
            SpawnedWeapon->SetOwner(this);

            if (Index == RightWeaponIndex) {
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

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, SpawnedWeapons, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentWeapon, Params);
}

void ABaseCharacter::GiveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& GameplayEffects) const
{
    if (!ensure(AbilitySystemComponent)) {
        return;
    }

    for (const auto& GameplayEffect : GameplayEffects) {
        if (!GameplayEffect)
            continue;

        FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
        if (NewHandle.IsValid()) {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
        }
    }
}

void ABaseCharacter::RemoveGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& EffectsToRemove) const
{
    if (!ensure(AbilitySystemComponent)) {
        return;
    }

    for (const auto& EffectToRemove : EffectsToRemove) {
        if (!EffectToRemove)
            continue;

        FGameplayEffectQuery Query;
        Query.EffectDefinition = EffectToRemove;

        AbilitySystemComponent->RemoveActiveEffects(Query, -1);
    }
}

bool ABaseCharacter::HasWeapon(EWeaponSlot WeaponSlot)
{
    if (WeaponSlot == EWeaponSlot::None)
        return false;

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

void ABaseCharacter::GiveGameplayAbilities(TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>> AbilitiesToGive)
{
    if (!ensure(AbilitySystemComponent)) {
        return;
    }

    if (HasAuthority() && !bAbilitiesInitialized) {
        // Grant abilities, but only on the server
        for (auto& GameplayAbility : AbilitiesToGive) {
            if (!GameplayAbility.Value)
                continue;

            FGameplayAbilitySpec AbilitySpec(GameplayAbility.Value, 1, StaticCast<int32>(GameplayAbility.Key), this);

            AbilitySystemComponent->GiveAbility(AbilitySpec);
        }

        bAbilitiesInitialized = true;
    }
}

void ABaseCharacter::RemoveGameplayAbility(const UGameplayAbility* AbilityToRemove)
{
    if (!ensure(AbilitySystemComponent) || !IsValid(AbilityToRemove)) {
        return;
    }

    FGameplayAbilitySpecHandle AbilityToRemoveSpecHandle(AbilityToRemove->GetCurrentAbilitySpecHandle());

    AbilitySystemComponent->ClearAbility(AbilityToRemoveSpecHandle);
}

bool ABaseCharacter::CanAttack() const
{
    if (!IsValid(AbilitySystemComponent))
        return false;

    return true;
}

bool ABaseCharacter::CanEquipWeapon() const
{
    if (SpawnedWeapons[LeftWeaponIndex]->GetWeaponType() == EWeaponType::TwoHanded || SpawnedWeapons[RightWeaponIndex]->GetWeaponType() == EWeaponType::TwoHanded)
        return false;

    return true;
}

void ABaseCharacter::TryUseAbility(EAbilityInputID AbilityInputID, bool bPressed)
{
    if (!ensure(AbilitySystemComponent))
        return;
    if (!GameplayAbilities.Contains(AbilityInputID) || !IsValid(GameplayAbilities[AbilityInputID].GetDefaultObject()))
        return;

    TSubclassOf<UGameplayAbility> AbilityClass = GameplayAbilities[AbilityInputID];

    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
    if (!ensure(Spec))
        return;

    if (bPressed) {
        AbilitySystemComponent->AbilityLocalInputPressed(Spec->InputID);
    } else {
        AbilitySystemComponent->AbilityLocalInputReleased(Spec->InputID);
    }
}