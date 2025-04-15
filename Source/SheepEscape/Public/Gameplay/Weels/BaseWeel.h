// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeel.generated.h"

class USphereComponent;
class UMainGameInstance;

UCLASS()
class SHEEPESCAPE_API ABaseWeel : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeel();
	void SetupMesh();
	void SetupOverlapSphere();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Blueprints Events */
	UFUNCTION(BlueprintImplementableEvent)
	void AllSheepsIn();
	UFUNCTION(BlueprintImplementableEvent)
	void CancelAllSheepsIn();

	UPROPERTY(BlueprintReadOnly)
	int NbSheepsInside;

private:
	UMainGameInstance* GameInstance;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	/** Overlap Sphere */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* OverlapSphere;

	void InitializeOverlapSphere();

	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void AddingAlreadyOverlappingCharacter();
	void ActorJoin(AActor* Actor);
	void ActorLeft(AActor* Actor);
};
