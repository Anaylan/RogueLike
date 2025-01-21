#include "Components/Manager/E_PlayerCameraManager.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Components/Controller/E_PlayerController.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Components/StaticMeshComponent.h"


AE_PlayerCameraManager::AE_PlayerCameraManager()
{
	//GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	//GameplayCamera->SetupAttachment(RootComponent);
}

void AE_PlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	//GameplayCameras.Debug.Enable
}

void AE_PlayerCameraManager::OnPossessed(AActor* InPawn)
{
	PossessedCharacter = Cast<APlayableCharacter>(InPawn);

	//GameplayCamera->ActivateCameraForPlayerController(GetOwningPlayerController());
}

void AE_PlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!PossessedCharacter.IsValid())
		return;
	OutVT.POV.Location = PossessedCharacter->GetActorLocation();
}