// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepBot.h"
#include "AIController.h"
#include "NavigationData.h"
#include "Navigation/PathFollowingComponent.h"
#include "Characters/SheepCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	VisualSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Visual Sphere"));
	VisualSphere->SetupAttachment(GetRootComponent());

	ProtectedSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Protected Sphere"));
	ProtectedSphere->SetupAttachment(GetRootComponent());
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();

	BotController = Cast<AAIController>(GetController());
	InitializeSphereOverlaps();
}

void ASheepBot::Tick(float DeltaTime)
{
	if (BotController && SheepInVisualRange.Num() > 0)
	{
		FVector DesiredPosition = Cohesion() + Separation();

		MoveToPosition(DesiredPosition);
	}
}

void ASheepBot::Eliminate()
{
	UE_LOG(LogTemp, Warning, TEXT("SheepBot Eliminated !"));
}

FVector ASheepBot::Cohesion()
{
	FVector HerdCenter = FVector(0, 0, 0);

	for (int i = 0; i < SheepInVisualRange.Num(); i++)
	{
		HerdCenter = SheepInVisualRange[i]->GetActorLocation();
	}

	HerdCenter /= SheepInVisualRange.Num();

	return HerdCenter;
}

FVector ASheepBot::Separation()
{
	FVector Avoidance = FVector(0, 0, 0);

	for (int i = 0; i < SheepInProtectedRange.Num(); i++)
	{
		Avoidance += GetActorLocation() - SheepInProtectedRange[i]->GetActorLocation();
	}

	return Avoidance;
}

void ASheepBot::MoveToPosition(FVector& DesiredPosition)
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(DesiredPosition);
	MoveRequest.SetAcceptanceRadius(1.f);

	BotController->MoveTo(MoveRequest);
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
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)) && OtherActor != this)
	{
		SheepInVisualRange.Add(OtherActor);
	}
}

void ASheepBot::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)) && OtherActor != this)
	{
		SheepInVisualRange.Remove(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)) && OtherActor != this)
	{
		SheepInProtectedRange.Add(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)) && OtherActor != this)
	{
		SheepInProtectedRange.Remove(OtherActor);
	}
}
