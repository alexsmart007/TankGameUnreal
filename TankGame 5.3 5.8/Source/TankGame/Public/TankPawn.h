// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankPawn.generated.h"

class AShell;
class UBoxComponent;
class USoundBase;
class UStaticMeshComponent;

/**
 * Shared tank body, driving and cannon (Unity's ShootControl) for the player and enemy tanks.
 */
UCLASS(Abstract)
class TANKGAME_API ATankPawn : public APawn
{
	GENERATED_BODY()

public:
	ATankPawn();

	// Drive along the tank's forward vector; Scale is -1 (reverse) to 1 (forward). Slides along walls.
	void MoveForward(float Scale, float DeltaTime);

	// Turn in place; Scale is -1 (left) to 1 (right)
	void Turn(float Scale, float DeltaTime);

	// Turn toward a world direction at RotateSpeed; returns the angle (degrees) still left to turn
	float TurnTowards(const FVector& Direction, float DeltaTime);

	// True when the cooldown has passed and the tank has fewer than MaxActiveShells in flight
	UFUNCTION(BlueprintPure, Category = "Tank")
	bool CanFire() const;

	// Spawn a shell at FirePoint, facing the way the tank faces. Returns null if the tank can't fire yet.
	UFUNCTION(BlueprintCallable, Category = "Tank")
	AShell* Fire();

	// Called by a shell this tank fired when it is destroyed
	void NotifyShellRemoved() { ActiveShells = FMath::Max(0, ActiveShells - 1); }

	FVector GetFirePointLocation() const;

	float GetMoveSpeed() const { return MoveSpeed; }

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Audio")
	TObjectPtr<USoundBase> FireSound;

private:
	int32 ActiveShells = 0;
	float LastFireTime = -1000.f;
};
