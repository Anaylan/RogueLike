// Copyright Epic Games, Inc. All Rights Reserved.

#include "RogueLike.h"
#include "Modules/ModuleManager.h"

#if ALLOW_CONSOLE
#include "Engine/Console.h"
#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, RogueLike, "RogueLike");

#define LOCTEXT_NAMESPACE "RogueLikeModule"

void FRogueLikeModule::StartupModule()
{
    FDefaultGameModuleImpl::StartupModule();

#if ALLOW_CONSOLE
    UConsole::RegisterConsoleAutoCompleteEntries.AddRaw(this, &FRogueLikeModule::Console_OnRegisterAutoCompleteEntries);
#endif
}

void FRogueLikeModule::ShutdownModule()
{
#if ALLOW_CONSOLE
    UConsole::RegisterConsoleAutoCompleteEntries.RemoveAll(this);
#endif

    FDefaultGameModuleImpl::ShutdownModule();
}

#if ALLOW_CONSOLE
void FRogueLikeModule::Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands)
{
    const auto CommandColor { GetDefault<UConsoleSettings>()->AutoCompleteCommandColor };

    auto* Command { &AutoCompleteCommands.AddDefaulted_GetRef() };
    Command->Command = FString { TEXT("ShowDebug Hits") };
    Command->Desc = FString { TEXT("Displays hit impact") };
    Command->Color = CommandColor;
}
#endif

#undef LOCTEXT_NAMESPACE
