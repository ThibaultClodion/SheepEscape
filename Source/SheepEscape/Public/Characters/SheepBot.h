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

	/** Sphere Overlap */
	void InitializeSphereOverlaps();

	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Boids */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Boids")
	float CohesionFactor;
	UPROPERTY(EditDefaultsOnly, Category = "Boids")
	float AlignmentFactor;
	UPROPERTY(EditDefaultsOnly, Category = "Boids")
	float RandomNoiseFactor;

private:

	/** Boids */
	FVector Cohesion();
	FVector Alignment();

	void AddingAlreadyOverlappingSheeps();
	void AddSheepInVisualRange(AActor*& OtherActor);
	void RemoveSheepInVisualRange(AActor*& OtherActor);
	UPROPERTY()
	TArray<ABaseCharacter*> SheepInVisualRange;
	float VisualSphereRadius = 750.f;
};
