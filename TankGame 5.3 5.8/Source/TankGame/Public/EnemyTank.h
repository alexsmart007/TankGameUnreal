// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "EnemyTank.generated.h"

/**
 * Enemy tank (Unity's EnemyAI): spins in place until its sight line hits the player, then faces and fires.
 */
UCLASS()
class TANKGAME_API AEnemyTank : public ATankPawn
{
	GENERATED_BODY()

public:
	AEnemyTank();

	virtual void Tick(float DeltaTime) override;

protected:
	// Unity raycast length 25 (m) = 2500 cm
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float SightDistance = 2500.f;

	// Degrees per second while searching (Unity turned 0.5 degrees per frame, about 30 at 60 fps)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float ScanSpeed = 30.f;

	// Height above the tank's origin the sight line starts from
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	float SightHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|AI")
	bool bDrawSightLine = false;
};
