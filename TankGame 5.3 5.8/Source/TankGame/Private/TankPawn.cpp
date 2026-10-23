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

void ATankPawn::MoveForward(float Scale, float DeltaTime)
{
	const FVector Delta = GetActorForwardVector() * (Scale * MoveSpeed * DeltaTime);
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	if (Hit.bStartPenetrating)
	{
		// A sweep that starts inside something (usually the floor) blocks every move,
		// so push out of it and try again
		AddActorWorldOffset(Hit.Normal * (Hit.PenetrationDepth + 0.1f));
		AddActorWorldOffset(Delta, true);
	}
	else if (Hit.bBlockingHit)
	{
		// Slide along the wall with the rest of the move instead of stopping dead
		FVector Slide = FVector::VectorPlaneProject(Delta * (1.f - Hit.Time), Hit.Normal);
		Slide.Z = 0.f;
		AddActorWorldOffset(Slide, true);
	}
}

void ATankPawn::Turn(float Scale, float DeltaTime)
{
	AddActorWorldRotation(FRotator(0.f, Scale * RotateSpeed * DeltaTime, 0.f));
}

float ATankPawn::TurnTowards(const FVector& Direction, float DeltaTime)
{
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, Direction.Rotation().Yaw);
	const float MaxStep = RotateSpeed * DeltaTime;
	const float Step = FMath::Clamp(DeltaYaw, -MaxStep, MaxStep);
	AddActorWorldRotation(FRotator(0.f, Step, 0.f));
	return FMath::Abs(DeltaYaw - Step);
}

bool ATankPawn::CanFire() const
{
	const bool bCooledDown = GetWorld()->GetTimeSeconds() - LastFireTime >= FireCooldown;
	const bool bShellAvailable = MaxActiveShells <= 0 || ActiveShells < MaxActiveShells;
	return ShellClass && bCooledDown && bShellAvailable;
}

AShell* ATankPawn::Fire()
{
	if (!ShellClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no ShellClass set."), *GetName());
		return nullptr;
	}
	if (!CanFire())
	{
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

	++ActiveShells;
	LastFireTime = GetWorld()->GetTimeSeconds();
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	return Shell;
}

FVector ATankPawn::GetFirePointLocation() const
{
	return FirePoint->GetComponentLocation();
}
