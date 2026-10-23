// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankPawn.generated.h"

class AShell;
class ATankPawn;
class UBoxComponent;
class USoundBase;
class UStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTankDestroyed, ATankPawn* /*Tank*/);
DECLARE_MULTICAST_DELEGATE(FOnTankRoundReset);

/**
 * Shared tank body for the player and enemy tanks: driving, the cannon and the shells it fired,
 * and its own destruction. Listeners learn about a destroyed tank through OnTankDestroyed.
 */
UCLASS(Abstract)
class TANKGAME_API ATankPawn : public APawn
{
	GENERATED_BODY()

public:
	ATankPawn();

#pragma region /** Driving */
	// Drive along the tank's forward vector; Scale is -1 (reverse) to 1 (forward). Slides along walls.
	void MoveForward(float Scale, float DeltaTime);

	// Turn in place; Scale is -1 (left) to 1 (right)
	void Turn(float Scale, float DeltaTime);

	// Turn toward a world direction at RotateSpeed; returns the angle (degrees) still left to turn
	float TurnTowards(const FVector& Direction, float DeltaTime);

	float GetMoveSpeed() const { return MoveSpeed; }
#pragma endregion

#pragma region /** Cannon */
	// True when firing is enabled, the cooldown has passed, the tank has fewer than MaxActiveShells
	// in flight, and (with bWaitForBounce) its shells have all bounced
	UFUNCTION(BlueprintPure, Category = "Tank")
	bool CanFire() const;

	// Spawn a shell at FirePoint, facing the way the tank faces. Returns null if the tank can't fire.
	UFUNCTION(BlueprintCallable, Category = "Tank")
	AShell* Fire();

	// Turn the cannon off and on (off once a round is won or lost)
	void SetFiringEnabled(bool bEnabled) { bFiringEnabled = bEnabled; }

	FVector GetFirePointLocation() const;
#pragma endregion

#pragma region /** Life cycle */
	// A shell hit this tank: announce it through OnTankDestroyed, then destroy the tank
	void TakeHit();

	// Get ready for the round to restart after the player dies: clears this tank's shells
	virtual void ResetForNewRound();

	// Broadcast just before the tank is destroyed by a hit
	FOnTankDestroyed OnTankDestroyed;

	// Broadcast when ResetForNewRound runs
	FOnTankRoundReset OnRoundReset;
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Chassis;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Turret;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TracksLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TracksRight;

	// Where shells spawn; move it to the end of the barrel in the Blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FirePoint;

	// Unity speed 2.5 (m/s) = 250 cm/s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Movement")
	float MoveSpeed = 250.f;

	// Degrees per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Movement")
	float RotateSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	TSubclassOf<AShell> ShellClass;

	// Unity launch force 10 (m/s) = 1000 cm/s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	float ShellSpeed = 1000.f;

	// Seconds between shots; 0 = no cooldown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	float FireCooldown = 0.f;

	// Shells this tank may have in flight at once; 0 = unlimited
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	int32 MaxActiveShells = 0;

	// Can't fire again until the last shell has bounced off a wall (or hit something)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	bool bWaitForBounce = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Audio")
	TObjectPtr<USoundBase> FireSound;

private:
	// Shells this tank fired that are still flying (exploded or destroyed ones are skipped)
	int32 CountLiveShells(bool bOnlyUnbounced) const;

	TArray<TWeakObjectPtr<AShell>> FiredShells;
	float LastFireTime = -1000.f;
	bool bFiringEnabled = true;
	bool bHit = false;
};
