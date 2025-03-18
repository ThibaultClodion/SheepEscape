// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Camera.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ACamera::ACamera()
{
	PrimaryActorTick.bCanEverTick = true;

	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisualSphere"));
	SetRootComponent(VisualSphere);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 2500.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

void ACamera::BeginPlay()
{
	Super::BeginPlay();
}

void ACamera::UpdateCharacters()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), Characters);
	}
}

void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToCenterLocation(DeltaTime);
	ZoomToSeeCharacters();
}

void ACamera::MoveToCenterLocation(float DeltaTime)
{
	FVector NewLocation = FMath::Lerp(GetActorLocation(), UGameplayStatics::GetActorArrayAverageLocation(Characters), DeltaTime * 2);
	VisualSphere->SetWorldLocation(NewLocation);
}

void ACamera::ZoomToSeeCharacters()
{
	double MaxDistance = 0.0f;
	for (AActor* Actor : Characters)
	{
		if (FVector::Dist(Actor->GetActorLocation(), GetActorLocation()) > MaxDistance)
		{
			MaxDistance = FVector::Dist(Actor->GetActorLocation(), GetActorLocation());
		}
	}

	MaxDistance = FMath::Clamp(MaxDistance, MinArmLength, MaxArmLength);
	SpringArm->TargetArmLength = MaxDistance;
}

