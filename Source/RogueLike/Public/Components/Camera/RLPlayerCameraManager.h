#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "RLPlayerCameraManager.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API ARLPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	TWeakObjectPtr<class APlayableCharacter> PossessedCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;

public:
	ARLPlayerCameraManager();

	virtual void OnPossessed(AActor* InPawn);
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
