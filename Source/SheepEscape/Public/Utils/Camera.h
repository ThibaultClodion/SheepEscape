// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;

UCLASS()
class SHEEPESCAPE_API ACamera : public AActor
{
	GENERATED_BODY()
	
public:	
	ACamera();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateTargets();

	/** Define the targets of the camera */
	UPROPERTY(EditDefaultsOnly)
	bool AreCharactersTargets;
	UPROPERTY(EditDefaultsOnly)
	bool AreSignsTargets;

private:
	void MoveToCenterLocation(float DeltaTime);
	void ZoomToSeeTargets();
	float CustomDistanceWithRatio(FVector V1, FVector V2);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	float MinArmLength = 800.f;
	float MaxArmLength = 3000.f;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	TArray<AActor*> Targets;
};
