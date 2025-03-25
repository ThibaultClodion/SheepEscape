// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSign.generated.h"

class USphereComponent;
class ABasePlayer;

UCLASS()
class SHEEPESCAPE_API ABaseSign : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseSign();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Blueprints Events */
	UFUNCTION(BlueprintImplementableEvent)
	void AllPlayerIn();
	UFUNCTION(BlueprintImplementableEvent)
	void CancelAllPlayerIn();

	/** Overlap Sphere */
	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(BlueprintReadOnly)
	int PlayerIn = 0;

private:
	void AddingAlreadyOverlappingPlayer();

	void PlayerJoin(ABasePlayer* Player);
	void PlayerLeft(ABasePlayer* Player);
};
