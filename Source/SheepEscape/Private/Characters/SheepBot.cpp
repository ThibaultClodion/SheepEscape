// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepBot.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Characters/SheepCharacter.h"
#include "Characters/ShepherdCharacter.h"
#include "Components/SphereComponent.h"
#include "Managers/MainGameInstance.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	SetupVisualSphere();
}

void ASheepBot::SetupVisualSphere()
{
	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Visual Sphere"));
	VisualSphere->SetupAttachment(GetRootComponent());
	VisualSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VisualSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	VisualSphere->SetSphereRadius(MaxVisualRange);
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();

	RandomizeRange();
	InitializeSphereOverlaps();
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

	Super::Eliminate();

	if(PushedBy) PushedBy->AddEliminateSheepAction();

	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	GameInstance->SheepElimination();

	Destroy();
}

void ASheepBot::Move(float DeltaTime)
{
	UpdateBoidVelocity(DeltaTime);

	if (IsLeading)
	{
		LeadMovement();
	}
	else if (IsGazing)
	{
		GazingMovement();
	}
	else
	{
		BoidMovement();
	}
}

void ASheepBot::BoidMovement()
{
	if ((BoidVelocity / MaxSpeed).Length() >= MinVelocityLengthToMove)
	{
		AddMovementInput(BoidVelocity / MaxSpeed);
	}
	// If Velocity is too slow -> Stop movement and start gazing
	else
	{
		StartGazing();
	}
}

void ASheepBot::UpdateBoidVelocity(float DeltaTime)
{
	// Get Boid Behavior Velocity
	FVector TargetVelocity = BoidVelocity + Cohesion() + Separation() + Alignment();
	// Smooth velocity changes
	BoidVelocity = FMath::Lerp(BoidVelocity, TargetVelocity, DeltaTime * Acceleration);
	// Inertia
	BoidVelocity *= Inertia;
	// Clamp Size of Speed to MaxSpeed
	BoidVelocity = BoidVelocity.GetClampedToSize(0.f, MaxSpeed);
}

FVector ASheepBot::Cohesion()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	// Get the center of the herd
	FVector HerdCenter = FVector::ZeroVector;
	int nbSheepInVisualRange = 0;

	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		float Distance = FVector::Distance(GetActorLocation(), Sheep->GetActorLocation());
		if (Distance < VisualRange)
		{
			HerdCenter += Sheep->GetActorLocation();
			nbSheepInVisualRange++;
		}
	}

	if (nbSheepInVisualRange == 0) return FVector::ZeroVector;
	HerdCenter /= nbSheepInVisualRange;

	return (HerdCenter - GetActorLocation()) * CohesionFactor;
}

FVector ASheepBot::Separation()
{
	if (SheepInVisualRange.Num() == 0 && !Shepherd) return FVector::ZeroVector;

	//Separate from others sheeps
	FVector Move = FVector::ZeroVector;
	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		float Distance = FVector::Distance(GetActorLocation(), Sheep->GetActorLocation());
		if (Distance < AvoidRange)
		{
			// Avoid brutal collision
			Move += (GetActorLocation() - Sheep->GetActorLocation()).GetSafeNormal() * (AvoidRange - Distance);
		}
	}

	//Separate from shepherd
	if (Shepherd)
	{
		float Distance = FVector::Distance(GetActorLocation(), Shepherd->GetActorLocation());
		if (Distance < VisualRange)
		{
			Move += (GetActorLocation() - Shepherd->GetActorLocation()).GetSafeNormal() * (VisualRange - Distance);
		}
	}

	return Move * SeparationFactor;
}

FVector ASheepBot::Alignment()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	// Get the average velocity of the herd
	FVector AverageVelocity = FVector::ZeroVector;
	int nbSheepInVisualRange = 0;

	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		float Distance = FVector::Distance(GetActorLocation(), Sheep->GetActorLocation());
		if (Distance < VisualRange)
		{
			AverageVelocity += Sheep->GetVelocity();
			nbSheepInVisualRange++;
		}
	}

	if (nbSheepInVisualRange == 0) return FVector::ZeroVector;
	AverageVelocity /= nbSheepInVisualRange;

	// Alignment is more important when gazing
	if (IsGazing)
	{
		return (AverageVelocity - GetVelocity()) * GazingAlignmentFactor;
	}
	else
	{
		return (AverageVelocity - GetVelocity()) * AlignmentFactor;
	}
}

void ASheepBot::GazingMovement()
{
	// If Velocity is too high -> StopGazing to follow herd
	if ((BoidVelocity / MaxSpeed).Length() >= MinVelocityLengthToStopGazing)
	{
		InterruptGazing();
		AddMovementInput(BoidVelocity / MaxSpeed);
	}
	else
	{
		// Don't let velocity being incremented
		BoidVelocity = FVector::ZeroVector;

	}
}

void ASheepBot::StartGazing()
{
	// Stop all movements
	BoidVelocity = FVector::ZeroVector;
	GetCharacterMovement()->StopMovementImmediately();

	// Setup Gazing Timer
	IsGazing = true;
	const float GazingTime = FMath::RandRange(MinGazingTime, MaxGazingTime);
	GetWorldTimerManager().SetTimer(GazingTimer, this, &ASheepBot::StopGazing, GazingTime);
}

void ASheepBot::StopGazing()
{
	IsGazing = false;

	bool OneChanceOutOfTwenty = (FMath::RandRange(0, 20) == 0);

	// Try to lead herd to another location
	if (OneChanceOutOfTwenty)
	{
		UE_LOG(LogTemp, Warning, TEXT("SheepBot takes a big lead !"));
		StartLeading2(MinBigLeadDistance, MaxBigLeadDistance);
	}
	// Little move to gaze at another location
	else
	{
		StartLeading2(MinLittleLeadDistance, MaxLittleLeadDistance);
	}
}

void ASheepBot::InterruptGazing()
{
	IsGazing = false;
	GetWorldTimerManager().ClearTimer(GazingTimer);
}

void ASheepBot::LeadMovement()
{
	// If Velocity is too high -> StopLeading to follow herd
	if ((BoidVelocity / MaxSpeed).Length() >= MinVelocityLengthToStopLead)
	{
		StopLeading2();
		AddMovementInput(BoidVelocity / MaxSpeed);
	}
	else
	{
		// Don't let velocity being incremented
		BoidVelocity = FVector::ZeroVector;
	}
}

void ASheepBot::StartLeading2(float MinDistance, float MaxDistance)
{
	IsLeading = true;

	// Get AIController
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	// Get random location
	FVector Origin = GetActorLocation();
	FVector2D Rand2D = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(MinDistance, MaxDistance);
	FVector RawTarget = Origin + FVector(Rand2D.X, Rand2D.Y, 0.f);

	// Project point to navigation
	FNavLocation NavLocation;
	if (!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(RawTarget, NavLocation))
		return;

	// Move to location
	FAIRequestID MoveRequestID = AIController->MoveToLocation(NavLocation.Location, 20.f);

	// Bind to OnRequestFinished
	if (auto* PathComp = AIController->GetPathFollowingComponent())
	{
		PathComp->OnRequestFinished.RemoveAll(this); // Clear previous bindings
		PathComp->OnRequestFinished.AddUObject(this, &ASheepBot::OnMoveCompleted);
	}
}

void ASheepBot::StopLeading2()
{
	IsLeading = false;
	BoidVelocity = FVector::ZeroVector;
	RandomLeadingInput = FVector::ZeroVector;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
}

void ASheepBot::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Warning, TEXT("SheepBot est arrivé à destination !"));

		StopLeading2();
	}
}

void ASheepBot::InitializeSphereOverlaps()
{
	VisualSphere->OnComponentBeginOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereOverlap);
	VisualSphere->OnComponentEndOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereEndOverlap);

	AddingAlreadyOverlappingActors();
}

void ASheepBot::OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AddActorInVisualRange(OtherActor);
}

void ASheepBot::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveActorInVisualRange(OtherActor);
}

void ASheepBot::AddingAlreadyOverlappingActors()
{
	TArray<AActor*> AlreadyOverlappingActors;
	VisualSphere->GetOverlappingActors(AlreadyOverlappingActors);

	for (AActor* Actor : AlreadyOverlappingActors)
	{
		AddActorInVisualRange(Actor);
	}
}

void ASheepBot::AddActorInVisualRange(AActor*& OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (ASheepBot* SheepBot = Cast<ASheepBot>(OtherActor))
		{
			SheepInVisualRange.AddUnique(SheepBot);
		}
		else if(ASheepCharacter* SheepCharacter = Cast<ASheepCharacter>(OtherActor))
		{
			SheepInVisualRange.AddUnique(SheepCharacter);
		}
		else if (AShepherdCharacter* ShepherdCharacter = Cast<AShepherdCharacter>(OtherActor))
		{
			Shepherd = ShepherdCharacter;
		}
	}
}

void ASheepBot::RemoveActorInVisualRange(AActor*& OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (ASheepBot* SheepBot = Cast<ASheepBot>(OtherActor))
		{
			SheepInVisualRange.Remove(SheepBot);
		}
		else if (ASheepCharacter* SheepCharacter = Cast<ASheepCharacter>(OtherActor))
		{
			SheepInVisualRange.Remove(SheepCharacter);
		}
		else if (AShepherdCharacter* ShepherdCharacter = Cast<AShepherdCharacter>(OtherActor))
		{
			Shepherd = nullptr;
		}
	}
}

void ASheepBot::RandomizeRange()
{
	VisualRange = FMath::RandRange(MinVisualRange, MaxVisualRange);
	AvoidRange = FMath::RandRange(MinAvoidRange, MaxAvoidRange);
	GetWorldTimerManager().SetTimer(RandomizeRangeTimer, this, &ASheepBot::RandomizeRange, RandomizeTime);
}
