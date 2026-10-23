// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "EnemyTank.generated.h"

/**
 * Enemy tank body. Its behaviour lives in ATankAIController; the settings below tune it per Blueprint.
 */
UCLASS()
class TANKGAME_API AEnemyTank : public ATankPawn
{
	GENERATED_BODY()

	friend class ATankAIController;

public:
	AEnemyTank();

protected:
	// False keeps the tank in place, spinning to scan like the original Unity enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	bool bCanRoam = true;

	// How far the tank can see (Unity raycast length 25 m = 2500 cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float SightDistance = 2500.f;

	// Half-angle of the view cone in degrees (45 = 90 degree cone)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float SightAngle = 45.f;

	// Stops chasing and starts shooting inside this distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float AttackRange = 1800.f;

	// Fires once it is aimed within this many degrees of the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float AimTolerance = 5.f;

	// Roaming picks random reachable points within this distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float RoamRadius = 1500.f;

	// Random pause between roam points, in seconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	FVector2D RoamPause = FVector2D(0.5f, 2.f);

	// Seconds spent looking around the player's last known position before roaming again
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float SearchDuration = 3.f;

	// Degrees per second when scanning in place (Unity turned 0.5 degrees per frame, about 30 at 60 fps)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float ScanSpeed = 30.f;

	// Draw the AI state, view cone and path while playing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	bool bDrawDebug = false;
};
