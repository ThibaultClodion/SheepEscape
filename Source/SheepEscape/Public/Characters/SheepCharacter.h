// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BasePlayer.h"
#include "SheepCharacter.generated.h"


UCLASS()
class SHEEPESCAPE_API ASheepCharacter : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	ASheepCharacter();
	void SetupHeadbuttTraces();

	virtual void Eliminate() override;

	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;

	/** Actions */
	void AddEliminateShepherdAction();
	void AddEliminateSheepAction();
	UFUNCTION(BlueprintCallable)
	void AddSurviveAction();

protected:
	virtual void Hit() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeColor(int Health);


private:
	/** Attributes */
	float MaxSpeed = 450.f;
	int HealthPoints = 2;

	/** Heabutt */
	void Heabutt();
	void HeadButtTrace(TArray<FHitResult>& BoxHits);

	UPROPERTY(EditDefaultsOnly, Category = Headbutt)
	float HeadbuttForceScale = 500000.f;

	UPROPERTY(EditDefaultsOnly, Category = Headbutt)
	FVector HeadbuttTraceExtent = FVector(5.f);

	UPROPERTY(VisibleAnywhere, Category = Headbutt)
	USceneComponent* HeadbuttTraceStart;

	UPROPERTY(VisibleAnywhere, Category = Headbutt)
	USceneComponent* HeadbuttTraceEnd;
};
