// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EScoreAction : uint8
{
	ESA_SheepElimateSheep UMETA(DisplayName = "Sheep eliminate other sheep"),
	ESA_SheepElimateSheperd UMETA(DisplayName = "Sheep eliminate shepherd"),
	ESA_SheepNotIdentify  UMETA(DisplayName = "Sheep not identify"),
	ESA_ShepherdRescueSheep UMETA(DisplayName = "Shepherd rescue a sheep"),
	ESA_ShepherdEliminatePlayerSheep UMETA(DisplayName = "Shepherd eliminate a player sheep")
};