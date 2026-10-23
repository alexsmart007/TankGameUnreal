// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank.h"
#include "TankAIController.h"

AEnemyTank::AEnemyTank()
{
	AIControllerClass = ATankAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Perception looks out from the centre of the tank
	BaseEyeHeight = 0.f;

	MoveSpeed = 200.f;
	RotateSpeed = 90.f;

	// Replaces the Unity rule of one enemy shell in the world at a time
	FireCooldown = 1.5f;
	MaxActiveShells = 1;
}

void AEnemyTank::BeginPlay()
{
	Super::BeginPlay();
	SpawnTransform = GetActorTransform();
}

void AEnemyTank::ResetForNewRound()
{
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	Super::ResetForNewRound();
}
