// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShepherdCharacter.h"
#include "Characters/SheepCharacter.h"
#include "Characters/SheepBot.h"
#include "Managers/MainGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AShepherdCharacter::AShepherdCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	SetupCrookHitTraces();
}

void AShepherdCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShepherdCharacter::SetupCrookHitTraces()
{
	CrookHitTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace Start"));
	CrookHitTraceStart->SetupAttachment(GetRootComponent());
	CrookHitTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace End"));
	CrookHitTraceEnd->SetupAttachment(GetRootComponent());
}

void AShepherdCharacter::Eliminate()
{
	if (IsEliminate()) return;

	Super::Eliminate();

	if (PushedBy) PushedBy->AddEliminateShepherdAction();

	GameInstance->ShepherdElimination();
}

void AShepherdCharacter::Action(const FInputActionValue& Value)
{
	CrookHit();
}

void AShepherdCharacter::AddRescueSheepAction()
{
	GameInstance->AddAction(PlayerController, EScoreAction::ESA_ShepherdRescueSheep);
}

void AShepherdCharacter::AddEliminatePlayerAction()
{
	GameInstance->AddAction(PlayerController, EScoreAction::ESA_ShepherdEliminatePlayerSheep);
}

void AShepherdCharacter::CrookHit()
{
	TArray<FHitResult> BoxHits;
	CrookHitTrace(BoxHits);

	for (FHitResult HitResult : BoxHits)
	{
		//The hit actor is a sheep player
		if (ASheepCharacter* SheepCharacter = Cast<ASheepCharacter>(HitResult.GetActor()))
		{
			AddEliminatePlayerAction();
			SheepCharacter->Eliminate();
		}

		//The hit actor is a sheep bot
		if (ASheepBot* SheepBot = Cast<ASheepBot>(HitResult.GetActor()))
		{
			SheepBot->Eliminate();
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