// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Managers/MainGameInstance.h"
#include "BaseCharacter.generated.h"

class ASheepCharacter;
class UMainGameInstance;

UCLASS()
class SHEEPESCAPE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	/** Elimination */
	UFUNCTION(BlueprintCallable)
	virtual void Eliminate();

	UFUNCTION(BlueprintCallable)
	bool IsEliminate();

	/** Push */
    void Pushed(ASheepCharacter* SheepCharacter, FVector Direction, float HeadbuttForceScale);

	UPROPERTY(BlueprintReadOnly)
	ASheepCharacter* PushedBy;

protected:
	virtual void BeginPlay() override;

	UMainGameInstance* GameInstance;

	/** Push */
	void StartPushTimer(ASheepCharacter* Pusher);
	void StopPushTimer();
	void InterruptPushTimer();
	virtual void Hit();

	float PushedTime = 2.f;
	FTimerHandle PushedTimer;

	/** Stun */
	void StartStun();
	void StopStun();

	float StunTime = 1.f;
	float MaxSpeed;
	FTimerHandle StunTimer;
};