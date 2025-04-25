// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "SheepBot.generated.h"

class USphereComponent;
class AShepherdCharacter;

UCLASS()
class SHEEPESCAPE_API ASheepBot : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASheepBot();
	void SetupVisualSphere();

	virtual void Tick(float DeltaTime) override;

	virtual void Eliminate() override;

protected:
	virtual void BeginPlay() override;

private:
	/** Attributes */
	float MaxSpeed = 500.f;
	float Acceleration = 500.f;
};
