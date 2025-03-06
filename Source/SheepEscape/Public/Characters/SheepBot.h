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

	/** Overlaps */
	void InitializeSphereOverlaps();
	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void OnProtectedSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnProtectedSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Boids */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* ProtectedSphere;

private:

	/** Boids */
	FVector Cohesion();
	FVector Separation();

	UPROPERTY()
	TArray<AActor*> SheepInVisualRange;
	float VisualSphereRadius = 500.f;
	UPROPERTY()
	TArray<AActor*> SheepInProtectedRange;
	float ProtectedSphereRadius = 100.f;

	void InitializeBoidParameters();
	float CohesionFactor;
	float SeparationFactor;
	
};
