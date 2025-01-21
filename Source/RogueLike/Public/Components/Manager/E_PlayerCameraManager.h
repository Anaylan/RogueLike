

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"

#include "E_PlayerCameraManager.generated.h"

/**
 *
 */
UCLASS()
class ROGUELIKE_API AE_PlayerCameraManager : public APlayerCameraManager 
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    TWeakObjectPtr<class APlayableCharacter> PossessedCharacter;

public:
    AE_PlayerCameraManager();

    virtual void OnPossessed(AActor* InPawn);
    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
    virtual void UpdateCamera(float DeltaTime) override;
};