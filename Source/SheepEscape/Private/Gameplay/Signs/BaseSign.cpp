// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Signs/BaseSign.h"
#include "Components/SphereComponent.h"
#include "Characters/BasePlayer.h"
#include "Kismet/GameplayStatics.h"


ABaseSign::ABaseSign()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupOverlapSphere();
}

void ABaseSign::SetupOverlapSphere()
{
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Sphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ABaseSign::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeOverlapSphere();
	AddingAlreadyOverlappingPlayer();
}

void ABaseSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseSign::InitializeOverlapSphere()
{
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseSign::OnVisualSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseSign::OnVisualSphereEndOverlap);
}

void ABaseSign::OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABasePlayer* Player = Cast<ABasePlayer>(OtherActor))
	{
		PlayerJoin(Player);
	}
}

void ABaseSign::OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABasePlayer* Player = Cast<ABasePlayer>(OtherActor))
	{
		PlayerLeft(Player);
	}
}

void ABaseSign::AddingAlreadyOverlappingPlayer()
{
	TArray<AActor*> AlreadyOverlappingActors;
	OverlapSphere->GetOverlappingActors(AlreadyOverlappingActors);

	for (AActor* Actor : AlreadyOverlappingActors)
	{
		if (ABasePlayer* Player = Cast<ABasePlayer>(Actor))
		{
			PlayerJoin(Player);
		}
	}
}

void ABaseSign::PlayerJoin(ABasePlayer* Player)
{
	PlayerIn++;

	TArray<AActor*> Players;
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), Players);
	}

	if (PlayerIn == Players.Num())
	{
		AllPlayerIn();
	}
}

void ABaseSign::PlayerLeft(ABasePlayer* Player)
{
	PlayerIn--;

	TArray<AActor*> Players;
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), Players);
	}

	if (PlayerIn == Players.Num() - 1)
	{
		CancelAllPlayerIn();
	}
}