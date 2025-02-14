#include "Components/Camera/RLPlayerCameraManager.h"
#include "Actors/Characters/PlayableCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"


ARLPlayerCameraManager::ARLPlayerCameraManager()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	//GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	//GameplayCamera->SetupAttachment(RootComponent);
}

void ARLPlayerCameraManager::OnPossessed(AActor* InPawn)
{
	PossessedCharacter = Cast<APlayableCharacter>(InPawn);

	//GameplayCamera->ActivateCameraForPlayerController(GetOwningPlayerController());
}

void ARLPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}
