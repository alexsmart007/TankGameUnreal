// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Shell.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"

ATankPawn::ATankPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->InitBoxExtent(FVector(120.f, 90.f, 60.f));
	Collision->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = Collision;

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(Collision);
	Chassis->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret"));
	Turret->SetupAttachment(Chassis);
	Turret->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TracksLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracksLeft"));
	TracksLeft->SetupAttachment(Chassis);
	TracksLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TracksRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracksRight"));
	TracksRight->SetupAttachment(Chassis);
	TracksRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Unity fire offset (0, 1.7, 1.35) from the ground, converted to cm and to the box centre
	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(Collision);
	FirePoint->SetRelativeLocation(FVector(135.f, 0.f, 110.f));
}

AShell* ATankPawn::Fire()
{
	if (!ShellClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no ShellClass set."), *GetName());
		return nullptr;
	}

	const FTransform SpawnTransform(GetActorRotation(), FirePoint->GetComponentLocation());
	AShell* Shell = GetWorld()->SpawnActorDeferred<AShell>(ShellClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Shell)
	{
		return nullptr;
	}
	Shell->SetSpeed(ShellSpeed);
	Shell->FinishSpawning(SpawnTransform);

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	return Shell;
}
