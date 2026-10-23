// Fill out your copyright notice in the Description page of Project Settings.


#include "Shell.h"
#include "PlayerTank.h"
#include "TankGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AShell::AShell()
{
	InitialLifeSpan = 10.f;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(15.f);
	Collision->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Flat, gravity-free flight that reflects off walls like the Unity shells
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.f;
	ProjectileMovement->Friction = 0.f;
	ProjectileMovement->bConstrainToPlane = true;
	ProjectileMovement->SetPlaneConstraintNormal(FVector::UpVector);
}

void AShell::SetSpeed(float Speed)
{
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

void AShell::BeginPlay()
{
	Super::BeginPlay();

	bFiredByPlayer = GetInstigator() && GetInstigator()->IsA<APlayerTank>();

	// Don't hit the tank that fired until the shell has bounced
	if (GetInstigator())
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
	}

	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AShell::HandleBounce);
}

void AShell::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Free up a shell slot on the tank that fired this one
	ATankPawn* Shooter = Cast<ATankPawn>(GetOwner());
	if (IsValid(Shooter))
	{
		Shooter->NotifyShellRemoved();
	}

	Super::EndPlay(EndPlayReason);
}

void AShell::HandleBounce(const FHitResult& Hit, const FVector& ImpactVelocity)
{
	if (bExploded)
	{
		return;
	}

	if (ATankPawn* Tank = Cast<ATankPawn>(Hit.GetActor()))
	{
		const bool bHitPlayer = Tank->IsA<APlayerTank>();
		if (bHitPlayer != bFiredByPlayer || bBounced)
		{
			if (ATankGameMode* GameMode = GetWorld()->GetAuthGameMode<ATankGameMode>())
			{
				GameMode->TankDestroyed(Tank);
			}
			else
			{
				Tank->Destroy();
			}
			Explode();
			return;
		}
	}
	else if (bBounced)
	{
		Explode();
		return;
	}

	// First bounce: from now on the shell can also hit the tank that fired it
	bBounced = true;
	if (GetInstigator())
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), false);
	}
	UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
}

void AShell::Explode()
{
	bExploded = true;

	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());
	}
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	// Unity destroyed the shell 0.15 s after the explosion
	ProjectileMovement->StopSimulating(FHitResult());
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(0.15f);
}
