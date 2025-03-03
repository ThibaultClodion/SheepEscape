// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BasePlayer.h"
#include "SheepCharacter.generated.h"


UCLASS()
class SHEEPESCAPE_API ASheepCharacter : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	ASheepCharacter();

	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;

};
