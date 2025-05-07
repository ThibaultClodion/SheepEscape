// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Managers/MainGameInstance.h"
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
	UMainGameInstance* GameInstance;

	void MoveToCenterLocation(float DeltaTime);
	void ZoomToSeeTargets();
	float CustomDistanceWithRatio(FVector V1, FVector V2);

	TArray<AActor*> Targets;
	AActor* Shepherd;

	/** Visual Sphere */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	/** Spring Arm */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom Parameters")
	float MinArmLength = 1800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom Parameters")
	float MaxArmLength = 2400.f;

	/** View Camera */
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	/** Camera Parameters */
	UPROPERTY(EditDefaultsOnly, Category = "Follow Parameters")
	float ShepherdVelocityFollowFactor = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Follow Parameters")
	float MovementLerpFactor = 2.f;
};
