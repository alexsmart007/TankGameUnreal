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
 * Shared tank body and cannon (Unity's ShootControl) for the player and enemy tanks.
 */
UCLASS(Abstract)
class TANKGAME_API ATankPawn : public APawn
{
	GENERATED_BODY()

public:
	ATankPawn();

	// Spawn a shell at FirePoint, facing the way the tank faces
	UFUNCTION(BlueprintCallable, Category = "Tank")
	AShell* Fire();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	TSubclassOf<AShell> ShellClass;

	// Unity launch force 10 (m/s) = 1000 cm/s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Shooting")
	float ShellSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Audio")
	TObjectPtr<USoundBase> FireSound;
};
