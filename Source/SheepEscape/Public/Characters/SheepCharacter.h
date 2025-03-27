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

	/** Inputs */
	virtual void Action(const FInputActionValue& Value) override;

private:
	/** Attributes */
	float MaxSpeed = 500.f;

	/** Heabutt */
	void Heabutt();
	void HeadButtTrace(TArray<FHitResult>& HitResults);

	UPROPERTY(EditDefaultsOnly, Category = Headbutt)
	float HeadbuttForceScale = 500000.f;

	UPROPERTY(EditDefaultsOnly, Category = Headbutt)
	FVector HeadbuttTraceExtent = FVector(5.f);

	UPROPERTY(VisibleAnywhere, Category = Headbutt)
	USceneComponent* HeadbuttTraceStart;

	UPROPERTY(VisibleAnywhere, Category = Headbutt)
	USceneComponent* HeadbuttTraceEnd;

};
