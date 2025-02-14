// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Characters/EnemyCharacter.h"
#include "Core/Abilities/Attributes/PawnAttributeSet.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<URLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UPawnAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AbilitySystemComponent))
		return;
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	DamagedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetReceivedDamageAttribute()).AddUObject(this, &ThisClass::OnDamaged);	

	GiveGameplayAbilities(GameplayAbilities);
	GiveGameplayEffects(PassiveGameplayEffects);
}

void AEnemyCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Display, TEXT("%s, %f"), *Data.Attribute.GetName(), Data.NewValue);
	if (!IsAlive())
	{
		OnDeath();
		UE_LOG(LogTemp, Display, TEXT("Character %s is died"), *GetFullName())
	}
}

float AEnemyCharacter::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}

	return 0.f;
}

float AEnemyCharacter::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}

	return 0.f;
}

float AEnemyCharacter::GetReceivedDamage() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetReceivedDamage();
	}

	return 0.f;
}

float AEnemyCharacter::GetStrength() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetStrength();
	}

	return 0.f;
}

bool AEnemyCharacter::IsAlive() const
{
	return GetHealth() > 0.f;
}
