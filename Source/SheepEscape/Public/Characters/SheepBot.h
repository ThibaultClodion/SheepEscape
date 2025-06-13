// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Data/SheepBotData.h"
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
	virtual void Hit() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeColor(int Health);

private:

	// Elimination
	ASheepCharacter* GetNearestSheepCharacter();
	int HealthPoints = 2;

	// Parameters
	float MaxSpeed = 450.f;
	float EmotionalState = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Data")
	USheepBotData* SheepBotData;

	/** Movements */
	void Move(float DeltaTime);

	/** Velocity Update */
	void UpdateVelocity(float DeltaTime);
	FVector ComputeVelocity();
	FVector Velocity = FVector::ZeroVector;

	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	FVector Escape();
	void EmotionalStateUpdate(float DeltaTime);
	float CloseToShepherdNormalize(float DistanceToShepherd);
	float NormalizeVelocity();
};
