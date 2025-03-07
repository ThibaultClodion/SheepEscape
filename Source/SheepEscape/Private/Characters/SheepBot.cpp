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
		FVector TargetVelocity = BoidVelocity + Cohesion() + Separation() + Alignment();
		BoidVelocity = FMath::Lerp(BoidVelocity, TargetVelocity, DeltaTime * Acceleration);
		BoidVelocity *= 0.95f;
		BoidVelocity = BoidVelocity.GetClampedToSize(0.f, MaxSpeed);

		UE_LOG(LogTemp, Warning, TEXT("x : %lf, y : %lf, z : %lf"), BoidVelocity.X / MaxSpeed, BoidVelocity.Y / MaxSpeed, BoidVelocity.Z / MaxSpeed);

		AddMovementInput(BoidVelocity / MaxSpeed);
	}
}

void ASheepBot::Eliminate()
{
	UE_LOG(LogTemp, Warning, TEXT("SheepBot Eliminated !"));
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
		if (FVector::Distance(GetActorLocation(), Sheep->GetActorLocation()) < AvoidDistance)
		{
			Move += (GetActorLocation() - Sheep->GetActorLocation()).GetSafeNormal() * (AvoidDistance - FVector::Distance(GetActorLocation(), Sheep->GetActorLocation()));
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
