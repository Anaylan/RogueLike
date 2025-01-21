#include "Actors/Characters/PlayableCharacter.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "AbilitySystemComponent.h"
#include "Core/Player/BasePlayerState.h"

APlayableCharacter::APlayableCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	GameplayCamera->SetupAttachment(RootComponent);
	GameplayCamera->SetIsReplicated(true);
}

void APlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABasePlayerState* BasePlayerState{ GetPlayerState<ABasePlayerState>() };
	if (!ensure(BasePlayerState))
		return;

	AbilitySystemComponent = Cast<UBaseAbilitySystemComponent>(BasePlayerState->GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(BasePlayerState, this);
		AttributeSet = BasePlayerState->GetAttributeSet();
		GiveGameplayAbilities(GameplayAbilities);
	}

	GiveGameplayEffects(PassiveGameplayEffects);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		GameplayCamera->ActivateCameraForPlayerController(PC);
	}
}

void APlayableCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ABasePlayerState* BasePlayerState{ GetPlayerState<ABasePlayerState>() };
	if (!ensure(BasePlayerState))
		return;

	AbilitySystemComponent = Cast<UBaseAbilitySystemComponent>(BasePlayerState->GetAbilitySystemComponent());
	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(BasePlayerState, this);
	AttributeSet = BasePlayerState->GetAttributeSet();

	GiveGameplayEffects(PassiveGameplayEffects);
}
