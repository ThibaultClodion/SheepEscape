// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepBot.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Characters/SheepCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Visual Sphere"));
	VisualSphere->SetupAttachment(GetRootComponent());
	VisualSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VisualSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	VisualSphere->SetSphereRadius(VisualSphereRadius);

	ProtectedSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Protected Sphere"));
	ProtectedSphere->SetupAttachment(GetRootComponent());
	ProtectedSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ProtectedSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ProtectedSphere->SetSphereRadius(ProtectedSphereRadius);
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();

	InitializeSphereOverlaps();
}

void ASheepBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector MoveInput = Cohesion() + Separation() + Alignment();

	if (MoveInput.IsNearlyZero())
	{
		//TODO : NoMoveBehavior
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("x : %lf, y : %lf, z : %lf"), MoveInput.X, MoveInput.Y, MoveInput.Z);
		AddMovementInput(MoveInput);
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
	for (AActor* Sheep : SheepInVisualRange)
	{
		HerdCenter += Sheep->GetActorLocation();
	}

	HerdCenter /= SheepInVisualRange.Num();

	return (HerdCenter - GetActorLocation()) * CohesionFactor;
}

FVector ASheepBot::Separation()
{
	if (SheepInProtectedRange.Num() == 0) return FVector::ZeroVector;

	FVector AvoidLocation = FVector::ZeroVector;
	for (AActor* Sheep : SheepInProtectedRange)
	{
		AvoidLocation += GetActorLocation() - Sheep->GetActorLocation();
	}

	AvoidLocation /= SheepInProtectedRange.Num();

	return AvoidLocation * SeparationFactor;
}

FVector ASheepBot::Alignment()
{
	if (SheepInVisualRange.Num() == 0) return FVector::ZeroVector;

	FVector AverageSpeed = FVector::ZeroVector;
	for (AActor* Sheep : SheepInVisualRange)
	{
		AverageSpeed += Sheep->GetVelocity();
	}

	AverageSpeed /= SheepInVisualRange.Num();

	return (AverageSpeed - GetVelocity()) * AlignmentFactor;
}

void ASheepBot::InitializeSphereOverlaps()
{
	VisualSphere->OnComponentBeginOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereOverlap);
	VisualSphere->OnComponentEndOverlap.AddDynamic(this, &ASheepBot::OnVisualSphereEndOverlap);
	ProtectedSphere->OnComponentBeginOverlap.AddDynamic(this, &ASheepBot::OnProtectedSphereOverlap);
	ProtectedSphere->OnComponentEndOverlap.AddDynamic(this, &ASheepBot::OnProtectedSphereEndOverlap);
}

void ASheepBot::OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInVisualRange.Add(OtherActor);
	}
}

void ASheepBot::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInVisualRange.Remove(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInProtectedRange.Add(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInProtectedRange.Remove(OtherActor);
	}
}