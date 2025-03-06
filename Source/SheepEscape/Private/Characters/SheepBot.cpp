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
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();

	InitializeSphereOverlaps();
}

void ASheepBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASheepBot::Eliminate()
{
	UE_LOG(LogTemp, Warning, TEXT("SheepBot Eliminated !"));
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
