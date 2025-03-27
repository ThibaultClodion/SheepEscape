// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShepherdCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AShepherdCharacter::AShepherdCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	CrookHitTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace Start"));
	CrookHitTraceStart->SetupAttachment(GetRootComponent());
	CrookHitTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace End"));
	CrookHitTraceEnd->SetupAttachment(GetRootComponent());
}

void AShepherdCharacter::Action(const FInputActionValue& Value)
{
	CrookHit();
}

void AShepherdCharacter::CrookHit()
{
	TArray<FHitResult> BoxHits;
	CrookHitTrace(BoxHits);

	for (FHitResult HitResult : BoxHits)
	{
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
		{
			BaseCharacter->Eliminate();
		}
	}
}

void AShepherdCharacter::CrookHitTrace(TArray<FHitResult>& BoxHits)
{
	const FVector Start = CrookHitTraceStart->GetComponentLocation();
	const FVector End = CrookHitTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::BoxTraceMulti(
		this,
		Start,
		End,
		CrookHitTraceExtent,
		CrookHitTraceEnd->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHits,
		true
	);
}
