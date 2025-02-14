// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAutoCompleteCommand;

class ROGUELIKE_API FRogueLikeModule : public FDefaultGameModuleImpl {
public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;

private:
#if ALLOW_CONSOLE
    void Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);
#endif
};
