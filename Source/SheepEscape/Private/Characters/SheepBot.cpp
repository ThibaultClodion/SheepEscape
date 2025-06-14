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
		+ SheepBotData->EscapeFactor * EmotionalState * Escape();
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
	if (GameInstance->Shepherd)
	{
		float Distance = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (Distance <= SheepBotData->EscapeRadius)
		{
			return (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()) / Distance;
		}
	}

	return FVector::ZeroVector;
}

void ASheepBot::EmotionalStateUpdate(float DeltaTime)
{
	/* Assign EmotionalState to 0.f if the shepherd is not in the EscapeRadius
	* Assign EmotionalState to ~1.f if the shepherd is near the sheep */

	if (GameInstance->Shepherd)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (DistanceToShepherd <= SheepBotData->EscapeRadius)
		{
			float TargetEmotionalState = FMath::Clamp(CloseToShepherdNormalize(DistanceToShepherd) + NormalizeVelocity() / 2.f, 0, 1.f);
			EmotionalState = FMath::Lerp(EmotionalState, CloseToShepherdNormalize(DistanceToShepherd), SheepBotData->EmotionalIncreaseFactor * DeltaTime);
			return;
		}
	}

	// If shepherd not close or not present
	if ((Velocity / MaxSpeed).Length() >= SheepBotData->MinVelocity)
	{
		EmotionalState = FMath::Lerp(EmotionalState, NormalizeVelocity() / 2.f, SheepBotData->EmotionalIncreaseFactor * DeltaTime);
	}
	else
	{
		EmotionalState = FMath::Lerp(EmotionalState, 0.f, SheepBotData->EmotionalDecreaseFactor * DeltaTime);
	}
}

float ASheepBot::CloseToShepherdNormalize(float DistanceToShepherd)
{
	return DistanceToShepherd / SheepBotData->EscapeRadius;
}

float ASheepBot::NormalizeVelocity()
{
	return Velocity.Length() / MaxSpeed;
}