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
	void SetupViewCamera();
	void SetupSpringArm();
	void SetupVisualSphere();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateTargets();

private:
	void MoveToCenterLocation(float DeltaTime);
	void ZoomToSeeTargets();
	float CustomDistanceWithRatio(FVector V1, FVector V2);

	TArray<AActor*> Targets;

	/** Visual Sphere */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	/** Spring Arm */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly)
	float MinArmLength = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxArmLength = 3000.f;

	/** View Camera */
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
};
