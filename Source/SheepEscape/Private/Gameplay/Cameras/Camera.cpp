// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Cameras/Camera.h"
#include "Characters/BaseCharacter.h"
#include "Gameplay/Signs/BaseSign.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ACamera::ACamera()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupVisualSphere();
	SetupSpringArm();
	SetupViewCamera();
}

void ACamera::SetupViewCamera()
{
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

void ACamera::SetupSpringArm()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 2500.f;
}

void ACamera::SetupVisualSphere()
{
	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisualSphere"));
	SetRootComponent(VisualSphere);
}

void ACamera::BeginPlay()
{
	Super::BeginPlay();
}

void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToCenterLocation(DeltaTime);
	ZoomToSeeTargets();
}

void ACamera::UpdateTargets()
{
	TArray<AActor*> NewTargets;

	// Add Characters
	TArray<AActor*> CharacterTargets;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), CharacterTargets);

		for(AActor* Character : CharacterTargets)
		{
			if (!Character->IsHidden())
			{
				NewTargets.Add(Character);
			}
		}
	}

	//Add Signs
	TArray<AActor*> SignTargets;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseSign::StaticClass(), SignTargets);

		for (AActor* Sign : SignTargets)
		{
			NewTargets.Add(Sign);
		}
	}

	Targets = NewTargets;
}

void ACamera::MoveToCenterLocation(float DeltaTime)
{
	FVector NewLocation = FMath::Lerp(GetActorLocation(), UGameplayStatics::GetActorArrayAverageLocation(Targets), DeltaTime * 2);
	VisualSphere->SetWorldLocation(NewLocation);
}

void ACamera::ZoomToSeeTargets()
{
	double MaxDistance = 0.0f;
	for (AActor* Actor : Targets)
	{
		if (CustomDistanceWithRatio(GetActorLocation(), Actor->GetActorLocation()) > MaxDistance)
		{
			MaxDistance = CustomDistanceWithRatio(GetActorLocation(), Actor->GetActorLocation());
		}
	}

	MaxDistance = FMath::Clamp(MaxDistance, MinArmLength, MaxArmLength);
	SpringArm->TargetArmLength = MaxDistance;
}

float ACamera::CustomDistanceWithRatio(FVector V1, FVector V2)
{
	/* These offset are here to correct two things :
	* 
	* The 16:9 Ratio imposes that X is ~2x more important
	* The 70° Rotation from the camera imposes that if actor run toward the camera
	the dist must be more important to see him
	*
	*/
	float XDist = 0.f;
	if (V2.X > V1.X)
	{
		XDist = FMath::Square(V2.X - V1.X) * 2.5f;
	}
	else
	{
		XDist = FMath::Square(V2.X - V1.X) * 5.f;
	}

	float YDist = FMath::Square(V2.Y - V1.Y);

	return FMath::Sqrt(XDist + YDist);
}