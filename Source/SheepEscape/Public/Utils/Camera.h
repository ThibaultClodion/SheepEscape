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
	void UpdateCharacters();

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float MinArmLength = 1200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float MaxArmLength = 2500.f;

private:
	void MoveToCenterLocation(float DeltaTime);
	void ZoomToSeeCharacters();

	TArray<AActor*> Characters;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

};
