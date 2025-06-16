// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShepherdCharacter.h"
#include "Characters/SheepCharacter.h"
#include "Characters/SheepBot.h"
#include "Managers/MainGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AShepherdCharacter::AShepherdCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = WalkSpeed / 2.5f;

	SetupCrookHitTraces();
}

void AShepherdCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AShepherdCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpeed(DeltaTime);
	UpdateStamina(DeltaTime);
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

	if (PushedBy) PushedBy->AddEliminateShepherdAction();

	Super::Eliminate();
	GameInstance->ShepherdElimination();
}

void AShepherdCharacter::Action(const FInputActionValue& Value)
{
	CrookHit();
}

void AShepherdCharacter::StartRun(const FInputActionValue& Value)
{
	if (Stamina >= 1.f)
	{
		IsRunning = true;
		Stamina = 0.8f; // Avoid spam of the run action
	}
}

void AShepherdCharacter::StopRun(const FInputActionValue& Value)
{
	IsRunning = false;
}

void AShepherdCharacter::UpdateSpeed(float DeltaTime)
{
	const float TargetSpeed = IsRunning ? RunSpeed : WalkSpeed;
	auto* Movement = GetCharacterMovement();
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(Movement->MaxWalkSpeed, TargetSpeed, DeltaTime * Acceleration);
}

void AShepherdCharacter::UpdateStamina(float DeltaTime)
{
	if (IsRunning)
	{
		if (Stamina > 0.f)
		{
			Stamina -= DeltaTime / RunDuration;
		}
		else
		{
			StopRun(FInputActionValue());
		}
	}
	else if (Stamina < 1.f)
	{
		Stamina += DeltaTime / RunRecoverDuration;
	}
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
