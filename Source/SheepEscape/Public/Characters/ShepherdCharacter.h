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

	/** Actions */
	UFUNCTION(BlueprintCallable)
	void AddRescueSheepAction();
	void AddEliminatePlayerAction();

private:
	/** Attributes */
	float MaxSpeed = 700.f;

	/** CrookHit */
	void CrookHit();
	void CrookHitTrace(TArray<FHitResult>& BoxHits);

	UPROPERTY(EditDefaultsOnly, Category = CrookHit)
	FVector CrookHitTraceExtent = FVector(5.f);

	UPROPERTY(VisibleAnywhere, Category = CrookHit)
	USceneComponent* CrookHitTraceStart;

	UPROPERTY(VisibleAnywhere, Category = CrookHit)
	USceneComponent* CrookHitTraceEnd;
};
