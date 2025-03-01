// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "ShepherdCharacter.generated.h"

UCLASS()
class SHEEPESCAPE_API AShepherdCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AShepherdCharacter();
	
	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;
};
