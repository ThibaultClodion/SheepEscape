// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepBot.h"
#include "AIController.h"
#include "Characters/SheepCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

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
	FVector ForceInput = Cohesion();

	if (BotController)
	{
		AddMovementInput(ForceInput.GetSafeNormal());
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
		HerdCenter += SheepInVisualRange[i]->GetActorLocation();
	}

	HerdCenter /= SheepInVisualRange.Num();

	return HerdCenter - GetActorLocation();
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
	UE_LOG(LogTemp, Warning, TEXT("On Visual Entering : %s"), *OtherActor->GetName());

	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInVisualRange.Add(OtherActor);
	}
}

void ASheepBot::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("On Visual Quitting : %s"), *OtherActor->GetName());

	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInVisualRange.Remove(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("On Protected Entering : %s"), *OtherActor->GetName());

	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInProtectedRange.Add(OtherActor);
	}
}

void ASheepBot::OnProtectedSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("On Protected Quitting : %s"), *OtherActor->GetName());

	if (OtherActor && (Cast<ASheepBot>(OtherActor) || Cast<ASheepCharacter>(OtherActor)))
	{
		SheepInProtectedRange.Remove(OtherActor);
	}
}
