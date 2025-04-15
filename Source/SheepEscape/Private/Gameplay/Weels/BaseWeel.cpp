// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Weels/BaseWeel.h"
#include "Components/SphereComponent.h"
#include "Characters/BaseCharacter.h"
#include "Characters/SheepBot.h"
#include "Characters/SheepCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseWeel::ABaseWeel()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupMesh();
	SetupOverlapSphere();
}

void ABaseWeel::SetupMesh()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weel Mesh"));
	SetRootComponent(Mesh);
}

void ABaseWeel::SetupOverlapSphere()
{
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Sphere"));
	OverlapSphere->SetupAttachment(Mesh);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ABaseWeel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeel::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeOverlapSphere();
	AddingAlreadyOverlappingCharacter();
}

void ABaseWeel::InitializeOverlapSphere()
{
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeel::OnVisualSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeel::OnVisualSphereEndOverlap);
}

void ABaseWeel::OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ActorJoin(OtherActor);
}

void ABaseWeel::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorLeft(OtherActor);
}

void ABaseWeel::AddingAlreadyOverlappingCharacter()
{
	//Add a delay to make sure the actors are spawned
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			TArray<AActor*> AlreadyOverlappingActors;
			OverlapSphere->GetOverlappingActors(AlreadyOverlappingActors);
			for (AActor* Actor : AlreadyOverlappingActors)
			{
				ActorJoin(Actor);
			}
		}, 0.1f, false);
}

void ABaseWeel::ActorJoin(AActor* Actor)
{
	if (Cast<ASheepCharacter>(Actor) || Cast<ASheepBot>(Actor))
	{
		NbSheepsInside++;
	}

	TArray<AActor*> SheepsBot;
	TArray<AActor*> SheepsPlayer;
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepBot::StaticClass(), SheepsBot);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepCharacter::StaticClass(), SheepsPlayer);
	}

	if (NbSheepsInside == SheepsBot.Num() + SheepsPlayer.Num())
	{
		AllSheepsIn();
	}
}

void ABaseWeel::ActorLeft(AActor* Actor)
{
	if (Cast<ASheepCharacter>(Actor) || Cast<ASheepBot>(Actor))
	{
		NbSheepsInside--;
	}

	TArray<AActor*> SheepsBot;
	TArray<AActor*> SheepsPlayer;
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepBot::StaticClass(), SheepsBot);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepCharacter::StaticClass(), SheepsPlayer);
	}

	if (NbSheepsInside == SheepsBot.Num() + SheepsPlayer.Num() - 1)
	{
		CancelAllSheepsIn();
	}
}
