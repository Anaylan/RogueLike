// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.generated.h"

/**
 * Slots abilities
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8 
{
	NONE,
	Confirm UMETA(Hidden),
	Cancel UMETA(Hidden),
	IA_Jump UMETA(DisplayName = "Jump"),
	IA_Sprint UMETA(DisplayName = "Sprint"),
	IA_Walk UMETA(DisplayName = "Walk"),
	IA_Rotation UMETA(DisplayName = "Rotation"),
	IA_Equip UMETA(DisplayName = "Equip"),
	IA_Attack UMETA(DisplayName = "Attack")
};

/**
 * Movement gait
 */
UENUM(BlueprintType)
enum class EGait : uint8
{
	Walk,
	Run,
	Sprint
};

/**
 * 
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	OrientToMovement,
	Strafe
};

/**
 *
 */
UENUM(BlueprintType)
enum class ECMovementMode : uint8
{
	OnGround,
	InAir
};

/**
 *
 */
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle,
	Moving
};


UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default,
	Katana,
	Sword
};