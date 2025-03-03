// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BasePlayer.h"
#include "ShepherdCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SHEEPESCAPE_API AShepherdCharacter : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	AShepherdCharacter();

	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;

protected:

	virtual void Eliminate() override;
};
