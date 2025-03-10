// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepBot.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Characters/SheepCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Visual Sphere"));
	VisualSphere->SetupAttachment(GetRootComponent());
	VisualSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VisualSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	VisualSphere->SetSphereRadius(VisualSphereRadius);
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();

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
	UE_LOG(LogTemp, Warning, TEXT("SheepBot Eliminated !"));
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

FVector ASheepBot::Cohesion()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	FVector HerdCenter = FVector::ZeroVector;
	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		HerdCenter += Sheep->GetActorLocation();
	}
	HerdCenter /= SheepInVisualRange.Num();

	// Run to the herd center if it's far to visual radius
	if (FVector::Distance(GetActorLocation(), HerdCenter) > VisualSphereRadius*1.8f)
	{
		return (HerdCenter - GetActorLocation()).GetSafeNormal() * MaxSpeed;
	}

	return (HerdCenter - GetActorLocation()) * CohesionFactor;
}

FVector ASheepBot::Separation()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	FVector Move = FVector::ZeroVector;
	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		float Distance = FVector::Distance(GetActorLocation(), Sheep->GetActorLocation());
		if (Distance < AvoidDistance)
		{
			// Avoid brutal collision
			Move += (GetActorLocation() - Sheep->GetActorLocation()).GetSafeNormal() * (AvoidDistance - Distance);
		}
	}

	return Move * SeparationFactor;
}

FVector ASheepBot::Alignment()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	FVector AverageVelocity = FVector::ZeroVector;
	for (ABaseCharacter* Sheep : SheepInVisualRange)
	{
		AverageVelocity += Sheep->GetVelocity();
	}
	AverageVelocity /= SheepInVisualRange.Num();

	return (AverageVelocity - GetVelocity()) * AlignmentFactor;
}

void ASheepBot::UpdateBoidVelocity(float DeltaTime)
{
	// Get Boid Behavior Velocity
	FVector TargetVelocity = BoidVelocity + Cohesion() + Separation() + Alignment();
	// Smooth velocity changement
	BoidVelocity = FMath::Lerp(BoidVelocity, TargetVelocity, DeltaTime * Acceleration);
	// Inertia
	BoidVelocity *= 0.99f;
	// Clamp Size of Speed to MaxSpeed
	BoidVelocity = BoidVelocity.GetClampedToSize(0.f, MaxSpeed);
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
		// Lead longer
		StartLeading(1.5f);
		RandomLeadingInput = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.f).GetClampedToSize(0.8f, 1.f);
	}
	// Little move to gaze at another location
	else
	{
		StartLeading(1.f);
		RandomLeadingInput = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.f).GetClampedToSize(0, MinVelocityLengthToStopGazing);
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
	if ((BoidVelocity / MaxSpeed).Length() >= FMath::Max(MinVelocityLengthToStopGazing, RandomLeadingInput.Length()))
	{
		InterruptLeading();
		AddMovementInput(BoidVelocity / MaxSpeed);
	}
	else
	{
		// Don't let velocity being incremented
		BoidVelocity = FVector::ZeroVector;

		AddMovementInput(RandomLeadingInput);
	}
}

void ASheepBot::StartLeading(float LeadingTimeFactor)
{
	// Setup Leading Timer
	IsLeading = true;
	const float LeadingTime = FMath::RandRange(MinLeadingTime, MaxLeadingTime) * LeadingTimeFactor;
	GetWorldTimerManager().SetTimer(LeadingTimer, this, &ASheepBot::StopLeading, LeadingTime);
}

void ASheepBot::StopLeading()
{
	IsLeading = false;

	// Stop all movements
	BoidVelocity = FVector::ZeroVector;
	RandomLeadingInput = FVector::ZeroVector;
	GetCharacterMovement()->StopMovementImmediately();
}

void ASheepBot::InterruptLeading()
{
	IsLeading = false;
	RandomLeadingInput = FVector::ZeroVector;

	GetWorldTimerManager().ClearTimer(LeadingTimer);
}

void ASheepBot::InitializeSphereOverlaps()
{
	VisualSphere->OnComponentBeginOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereOverlap);
	VisualSphere->OnComponentEndOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereEndOverlap);

	AddingAlreadyOverlappingSheeps();
}

void ASheepBot::OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AddSheepInVisualRange(OtherActor);
}

void ASheepBot::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveSheepInVisualRange(OtherActor);
}

void ASheepBot::AddingAlreadyOverlappingSheeps()
{
	TArray<AActor*> AlreadyOverlappingSheeps;
	VisualSphere->GetOverlappingActors(AlreadyOverlappingSheeps);

	for (AActor* Sheep : AlreadyOverlappingSheeps)
	{
		AddSheepInVisualRange(Sheep);
	}
}

void ASheepBot::AddSheepInVisualRange(AActor*& OtherActor)
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
	}
}

void ASheepBot::RemoveSheepInVisualRange(AActor*& OtherActor)
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
	}
}
