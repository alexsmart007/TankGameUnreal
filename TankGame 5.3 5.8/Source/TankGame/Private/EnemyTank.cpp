// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank.h"
#include "PlayerTank.h"
#include "TankGameMode.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AEnemyTank::AEnemyTank()
{
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AEnemyTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Start = GetActorLocation() + FVector(0.f, 0.f, SightHeight);
	const FVector End = Start + GetActorForwardVector() * SightDistance;

	// Walls and other tanks block the line of sight
	FCollisionObjectQueryParams ObjectTypes;
	ObjectTypes.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyTankSight), false, this);

	FHitResult Hit;
	const bool bSeesPlayer = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectTypes, Params)
		&& Hit.GetActor() && Hit.GetActor()->IsA<APlayerTank>();

	if (bDrawSightLine)
	{
		DrawDebugLine(GetWorld(), Start, End, bSeesPlayer ? FColor::Green : FColor::Red);
	}

	if (bSeesPlayer)
	{
		FVector ToPlayer = Hit.ImpactPoint - GetActorLocation();
		ToPlayer.Z = 0.f;
		SetActorRotation(ToPlayer.Rotation());

		const ATankGameMode* GameMode = GetWorld()->GetAuthGameMode<ATankGameMode>();
		if (!GameMode || GameMode->CanEnemyFire())
		{
			Fire();
		}
	}
	else
	{
		AddActorWorldRotation(FRotator(0.f, ScanSpeed * DeltaTime, 0.f));
	}
}
