// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BasePlayer.h"
#include "ShepherdCharacter.generated.h"

UCLASS()
class SHEEPESCAPE_API AShepherdCharacter : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	AShepherdCharacter();

	virtual void Tick(float DeltaTime) override;
	void SetupCrookHitTraces();

	virtual void Eliminate() override;

	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;
	virtual void StartRun(const FInputActionValue& Value) override;
	virtual void StopRun(const FInputActionValue& Value) override;

	/** Actions */
	UFUNCTION(BlueprintCallable)
	void AddRescueSheepAction();
	void AddEliminatePlayerAction();

protected:
	virtual void BeginPlay() override;

private:
	/** Attributes */
	UPROPERTY(EditDefaultsOnly, Category = Speed)
	float WalkSpeed = 380.f;
	UPROPERTY(EditDefaultsOnly, Category = Speed)
	float RunSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = Speed)
	float Acceleration = 2.f;

	/** CrookHit */
	void CrookHit();
	void CrookHitTrace(TArray<FHitResult>& BoxHits);

	UPROPERTY(EditDefaultsOnly, Category = CrookHit)
	FVector CrookHitTraceExtent = FVector(5.f);

	UPROPERTY(VisibleAnywhere, Category = CrookHit)
	USceneComponent* CrookHitTraceStart;

	UPROPERTY(VisibleAnywhere, Category = CrookHit)
	USceneComponent* CrookHitTraceEnd;

	/** Running */
	void UpdateSpeed(float DeltaTime);
	void UpdateStamina(float DeltaTime);

	bool IsRunning = false;
	float Stamina = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = Run)
	float RunDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = Run)
	float RunRecoverDuration = 6.f;
};
