// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepBot.h"
#include "Characters/SheepCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();
}

void ASheepBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller)
	{
		Move(DeltaTime);
	}
}

void ASheepBot::Eliminate()
{
	if (IsEliminate()) return;

	if (PushedBy)
	{
		PushedBy->AddEliminateSheepAction();
	}
	else if(ASheepCharacter* SheepCharacter = GetNearestSheepCharacter())
	{
		if ((SheepCharacter->GetActorLocation() - GetActorLocation()).Length() <= SheepBotData->MaxDistanceToEarnScore)
		{
			SheepCharacter->AddEliminateSheepAction();
		}
	}

	Super::Eliminate();
	GameInstance->SheepElimination(this);
	Destroy();
}

void ASheepBot::Hit()
{
	HealthPoints--;
	ChangeColor(HealthPoints);

	if (HealthPoints == 0)
	{
		Eliminate();
	}
}

ASheepCharacter* ASheepBot::GetNearestSheepCharacter()
{
	TArray<AActor*> SheepCharacters;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepCharacter::StaticClass(), SheepCharacters);
	}

	ASheepCharacter* NearestCharacter = NULL;
	float MaxDistance = MAX_flt;
	for (AActor* Actor : SheepCharacters)
	{
		if (ASheepCharacter* SheepCharacter = Cast<ASheepCharacter>(Actor))
		{
			if (!SheepCharacter->IsHidden() && (SheepCharacter->GetActorLocation() - GetActorLocation()).Length() <= MaxDistance)
			{
				MaxDistance = (SheepCharacter->GetActorLocation() - GetActorLocation()).Length();
				NearestCharacter = SheepCharacter;
			}
		}
	}

	return NearestCharacter;
}

void ASheepBot::Move(float DeltaTime)
{
	UpdateVelocity(DeltaTime);
	AddMovementInput(Velocity / MaxSpeed);
}

void ASheepBot::UpdateVelocity(float DeltaTime)
{
	EmotionalStateUpdate(DeltaTime);

	// Smooth Interpolation to target velocity
	FVector TargetVelocity = ComputeVelocity();
	Velocity = FMath::VInterpTo(Velocity, TargetVelocity, DeltaTime, SheepBotData->Acceleration);
	Velocity *= SheepBotData->Inertia;

	// If too slow -> slown down to 0
	float velocityNormalize = NormalizeVelocity();
	if (velocityNormalize < SheepBotData->MinVelocity)
	{
		Velocity = FVector::ZeroVector;
	}
	else if (velocityNormalize < SheepBotData->MinDecelerationVelocity)
	{
		Velocity = FMath::VInterpTo(Velocity, FVector::ZeroVector, DeltaTime, SheepBotData->Deceleration);
	}

	// Else -> be sure to not be too fast
	else
	{
		Velocity = Velocity.GetClampedToSize(0.f, MaxSpeed);
	}
}

FVector ASheepBot::ComputeVelocity()
{
	return Velocity
		+ (SheepBotData->CohesionFactor + (SheepBotData->CohesionStressFactor - SheepBotData->CohesionFactor) * EmotionalState) * Cohesion()
		+ (SheepBotData->SeparationFactor + (SheepBotData->SeparationStressFactor - SheepBotData->SeparationFactor) * EmotionalState) * Separation()
		+ (SheepBotData->AlignmentFactor + (SheepBotData->AlignmentStressFactor - SheepBotData->AlignmentFactor) * EmotionalState) * Alignment()
		+ SheepBotData->EscapeFactor * Escape();
}

FVector ASheepBot::Cohesion()
{
	FVector AveragePosition = FVector::ZeroVector;
	int nbSheepInCohesionRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= SheepBotData->CohesionRadius)
		{
			nbSheepInCohesionRadius++;
			AveragePosition += Sheep->GetActorLocation();
		}
	}

	if (nbSheepInCohesionRadius == 0) return FVector::ZeroVector;

	return AveragePosition / nbSheepInCohesionRadius - GetActorLocation();
}

FVector ASheepBot::Separation()
{
	FVector SeparationVelocity = FVector::ZeroVector;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		float Distance = (GetActorLocation() - Sheep->GetActorLocation()).Length();
		if (Distance <= SheepBotData->SeparationRadius)
		{
			SeparationVelocity += (GetActorLocation() - Sheep->GetActorLocation()) * (SheepBotData->SeparationRadius - Distance) / Distance;
		}
	}

	return SeparationVelocity;
}

FVector ASheepBot::Alignment()
{
	FVector AlignmentVelocity = FVector::ZeroVector;
	int nbSheepsInAlignmentRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= SheepBotData->AlignmentRadius)
		{
			nbSheepsInAlignmentRadius++;
			AlignmentVelocity += Sheep->GetVelocity();
		}
	}

	if (nbSheepsInAlignmentRadius == 0) return FVector::ZeroVector;
	
	return AlignmentVelocity / nbSheepsInAlignmentRadius - GetVelocity();
}

FVector ASheepBot::Escape()
{
	if (GameInstance->Shepherd && GameInstance->Shepherd->GetVelocity().Length() >= 5.f)
	{
		FVector ToShepherd = GetActorLocation() - GameInstance->Shepherd->GetActorLocation();

		float Distance = ToShepherd.Length();

		if (Distance <= SheepBotData->EscapeRadius)
		{
			// Apply squared distance falloff to make escape force lighter at greater distances
			float DistanceFalloff = FMath::Square(CloseToShepherdNormalize(Distance));
			return (ToShepherd / Distance) * DistanceFalloff;
		}
	}

	return FVector::ZeroVector;
}

void ASheepBot::EmotionalStateUpdate(float DeltaTime)
{
	float VelocityEmotionalState = NormalizeVelocity() / 2.f;

	if (GameInstance->Shepherd && GameInstance->Shepherd->GetVelocity().Length() >= 5.f)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		float ShepherdEmotionalState = FMath::Min(CloseToShepherdNormalize(DistanceToShepherd) * 3.f, 0.5f);

		if (ShepherdEmotionalState >= VelocityEmotionalState)
		{
			EmotionalState = FMath::Lerp(EmotionalState, ShepherdEmotionalState, SheepBotData->EmotionalIncreaseFactor * DeltaTime);
			return;
		}
	}

	if (NormalizeVelocity() >= SheepBotData->MinVelocity)
	{
		EmotionalState = FMath::Lerp(EmotionalState, VelocityEmotionalState, SheepBotData->EmotionalIncreaseFactor * DeltaTime);
	}
	else
	{
		EmotionalState = FMath::Lerp(EmotionalState, 0.f, SheepBotData->EmotionalDecreaseFactor * DeltaTime);
	}
}

float ASheepBot::CloseToShepherdNormalize(float DistanceToShepherd)
{
	return FMath::Clamp(1 - DistanceToShepherd / SheepBotData->EscapeRadius, 0.f, 1.f);
}

float ASheepBot::NormalizeVelocity()
{
	return Velocity.Length() / MaxSpeed;
}