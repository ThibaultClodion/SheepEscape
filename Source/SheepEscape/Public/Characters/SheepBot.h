// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "SheepBot.generated.h"

class USphereComponent;

UCLASS()
class SHEEPESCAPE_API ASheepBot : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASheepBot();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void Eliminate() override;

	/** Boids Behavior */
	UPROPERTY(EditDefaultsOnly, Category=Boids)
	float CohesionFactor;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float SeparationFactor;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float AlignmentFactor;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float AvoidDistance;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float Acceleration;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float MinVelocityLengthToMove;
	UPROPERTY(EditDefaultsOnly, Category = Boids)
	float MinVelocityLengthToStopGazing;

	/** Boid Visual Sphere */
	void InitializeSphereOverlaps();

	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

private:
	/** Attributes */
	float MaxSpeed = 500.f;

	/** Boids Behavior */
	void Move(float DeltaTime);
	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	void UpdateBoidVelocity(float DeltaTime);
	FVector BoidVelocity;

	/** Gazing Behavior */
	void StartGazing();
	void StopGazing();
	void InterruptGazing();
	bool IsGazing;
	float MinGazingTime = 5.0f;
	float MaxGazingTime = 10.0f;
	FVector RandomGazingInput;
	FTimerHandle GazingTimer;

	/** Boid Visual Sphere */
	void AddingAlreadyOverlappingSheeps();
	void AddSheepInVisualRange(AActor*& OtherActor);
	void RemoveSheepInVisualRange(AActor*& OtherActor);

	TArray<ABaseCharacter*> SheepInVisualRange;
	float VisualSphereRadius = 300.f;
};
