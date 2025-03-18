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

}

