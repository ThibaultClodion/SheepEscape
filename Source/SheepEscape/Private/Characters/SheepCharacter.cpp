// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ASheepCharacter::ASheepCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	HeadbuttTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace Start"));
	HeadbuttTraceStart->SetupAttachment(GetRootComponent());
	HeadbuttTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace End"));
	HeadbuttTraceEnd->SetupAttachment(GetRootComponent());
}

void ASheepCharacter::Action(const FInputActionValue& Value)
{
	Heabutt();
}

void ASheepCharacter::Heabutt()
{
	TArray<FHitResult> BoxHits;
	HeadButtTrace(BoxHits);

	for (FHitResult HitResult : BoxHits)
	{
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
		{
			BaseCharacter->Pushed(this, (HitResult.Location - GetActorLocation()),HeadbuttForceScale);
		}
	}
}

void ASheepCharacter::HeadButtTrace(TArray<FHitResult>& BoxHits)
{
	const FVector Start = HeadbuttTraceStart->GetComponentLocation();
	const FVector End = HeadbuttTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::BoxTraceMulti(
		this,
		Start,
		End,
		HeadbuttTraceExtent,
		HeadbuttTraceEnd->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHits,
		true
	);
}
