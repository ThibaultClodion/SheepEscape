// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"


UCLASS()
class SHEEPESCAPE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	void Pushed(FVector Direction, float HeadbuttForceScale);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Eliminate();

protected:
	virtual void BeginPlay() override;
};