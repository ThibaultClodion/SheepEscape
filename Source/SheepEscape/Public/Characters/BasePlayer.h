// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "BasePlayer.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class SHEEPESCAPE_API ABasePlayer : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	/** Inputs */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Move(const FInputActionValue& Value);
	virtual void Action(const FInputActionValue& Value);
	virtual void StartRun(const FInputActionValue& Value);
	virtual void StopRun(const FInputActionValue& Value);

protected:
	virtual void BeginPlay() override;

	/** Essentials Variables */
	UPROPERTY(BlueprintReadOnly)
	APlayerController* PlayerController;

	/** Inputs */
	UFUNCTION(BlueprintCallable)
	void InitializeMappingContext();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BaseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RunAction;
};
