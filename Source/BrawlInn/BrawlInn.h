// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(Brawl, Log, All);

#if _MSC_VER
#define FUNC_NAME    TEXT(__FUNCTION__)
#define FUNC_NAMEF   FString(__FUNCTION__)
#else // FIXME - GCC?
#define FUNC_NAME    TEXT(__func__)
#endif

#define B(Format, ...) \
{ \
    SET_WARN_COLOR(COLOR_CYAN);\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    if (Msg == "") \
    { \
        UE_LOG(Brawl, Log, TEXT("%s() : %s"), FUNC_NAME, *GetNameSafe(this));\
    } \
    else \
    { \
        UE_LOG(Brawl, Log, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    } \
    CLEAR_WARN_COLOR();\
}

#define BLog(Format, ...) \
{ \
    SET_WARN_COLOR(COLOR_CYAN);\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(Brawl, Log, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define BWarn(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_YELLOW );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(Brawl, Warning, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define BError(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_RED );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(Brawl, Error, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define BScreen(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    if (Msg == "") \
    { \
        TCHAR StdMsg[MAX_SPRINTF] = TEXT(""); \
        FCString::Sprintf(StdMsg, TEXT("%s() : %s"), FUNC_NAME, *GetNameSafe(this)); \
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, StdMsg); \
    } \
    else \
    { \
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, *(FUNC_NAMEF + ": " + Msg)); \
	} \
}