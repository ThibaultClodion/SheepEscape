// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Managers/MainGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

ASheepCharacter::ASheepCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	SetupHeadbuttTraces();
}

void ASheepCharacter::SetupHeadbuttTraces()
{
	HeadbuttTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace Start"));
	HeadbuttTraceStart->SetupAttachment(GetRootComponent());
	HeadbuttTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Heabutt Trace End"));
	HeadbuttTraceEnd->SetupAttachment(GetRootComponent());
}

void ASheepCharacter::Eliminate()
{
	if (IsEliminate()) return;

	Super::Eliminate();

	if (PushedBy) PushedBy->AddEliminateSheepAction();

	GameInstance->SheepElimination(this);
	GameInstance->RemoveScore(Cast<APlayerController>(Controller)->NetPlayerIndex);
}

void ASheepCharacter::Action(const FInputActionValue& Value)
{
	Heabutt();
}

void ASheepCharacter::AddEliminateShepherdAction()
{
	GameInstance->AddAction(PlayerController, EScoreAction::ESA_SheepEliminateSheperd);
}

void ASheepCharacter::AddEliminateSheepAction()
{
	GameInstance->AddAction(PlayerController, EScoreAction::ESA_SheepEliminateSheep);
}

void ASheepCharacter::AddSurviveAction()
{
	GameInstance->AddAction(PlayerController, EScoreAction::ESA_SheepSurvive);
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