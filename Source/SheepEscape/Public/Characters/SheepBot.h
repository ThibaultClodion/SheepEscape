// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Data/SheepData.h"
#include "SheepBot.generated.h"

UCLASS()
class SHEEPESCAPE_API ASheepBot : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASheepBot();

	virtual void Tick(float DeltaTime) override;

	virtual void Eliminate() override;

protected:
	virtual void BeginPlay() override;

private:

	/** Data */
	UPROPERTY(EditDefaultsOnly, Category = "Sheep Data")
	USheepData* Data;
	float MaxSpeed = 450.f;
	float EmotionalState = 0.f;

	/** Movements */
	void Move(float DeltaTime);
	void BoidMovement();
	void GazingMovement();

	/** Velocity Update */
	void UpdateVelocity(float DeltaTime);
	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	FVector Escape();
	void EmotionalStateUpdate(float DeltaTime);

	FVector Velocity = FVector::ZeroVector;
	
	/** Gaze Parameters */
	void StartGaze();
	void SetGazeTimer(float min, float max);
	void StopGaze();

	bool IsGazing = false;
	FVector GazeVelocity = FVector::ZeroVector;
	FTimerHandle GazeTimer;
};
