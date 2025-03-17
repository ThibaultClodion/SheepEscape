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
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Character Function */
	virtual void Eliminate() override;


	/** Boid Visual Sphere */
	void InitializeSphereOverlaps();

	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

private:
	void Move(float DeltaTime);

	/** Attributes */
	float MaxSpeed = 500.f;
	float Acceleration = 500.f;
	float MinVelocityLengthToMove = 0.07f;

	/** Boids Behavior */
	void BoidMovement();

	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	float CohesionFactor = 0.05f;
	float SeparationFactor = 0.15f;
	float AlignmentFactor = 0.08f;


	void UpdateBoidVelocity(float DeltaTime);
	FVector BoidVelocity;

	/** Gazing Behavior */
	void GazingMovement();

	void StartGazing();
	void StopGazing();
	void InterruptGazing();
	bool IsGazing = false;

	float MinGazingTime = 2.0f;
	float MaxGazingTime = 20.0f;
	FTimerHandle GazingTimer;

	float MinVelocityLengthToStopGazing = 0.15f;

	/** Leading Behavior */
	void LeadMovement();

	void StartLeading(float LeadingTimeFactor);
	void StopLeading();
	void InterruptLeading();
	bool IsLeading = false;

	float MinLeadingTime = 1.f;
	float MaxLeadingTime = 5.0f;
	FTimerHandle LeadingTimer;

	FVector RandomLeadingInput = FVector::ZeroVector;

	/** Boid Visual Sphere */
	void AddingAlreadyOverlappingActors();
	void AddActorInVisualRange(AActor*& OtherActor);
	void RemoveActorInVisualRange(AActor*& OtherActor);

	TArray<ABaseCharacter*> SheepInVisualRange;
	AShepherdCharacter* Shepherd;
	float VisualSphereRadius = 300.f;
	float AvoidDistance = 200.f;
};
