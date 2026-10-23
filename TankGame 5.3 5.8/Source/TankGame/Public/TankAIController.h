// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TankAIController.generated.h"

class AEnemyTank;
class APlayerTank;
class UAISenseConfig_Sight;

UENUM(BlueprintType)
enum class ETankAIState : uint8
{
	Roam,
	Chase,
	Attack,
	Search
};

/**
 * Enemy tank brain: a state machine that roams the nav mesh, chases and shoots the player
 * once it is seen, and searches the last known position after losing sight.
 * Drives like a tank: turns toward each path point before driving forward.
 */
UCLASS()
class TANKGAME_API ATankAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATankAIController();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Tank AI")
	ETankAIState GetAIState() const { return State; }

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void SetAIState(ETankAIState NewState);

	void TickRoam(float DeltaTime);
	void TickChase(float DeltaTime);
	void TickAttack(float DeltaTime);
	void TickSearch(float DeltaTime);

	// Find a nav mesh path to Destination; false if there is none
	bool SetDestination(const FVector& Destination);

	// Drive along the current path; true once it is finished or given up
	bool FollowPath(float DeltaTime);

	// True if a shell fired now would reach the player without hitting a wall or another tank first
	bool HasClearShot() const;

	void DrawDebug() const;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<AEnemyTank> Tank;

	TWeakObjectPtr<APlayerTank> Target;
	bool bCanSeeTarget = false;
	FVector LastKnownLocation = FVector::ZeroVector;

	ETankAIState State = ETankAIState::Roam;

	TArray<FVector> PathPoints;
	int32 PathIndex = 0;
	float PathTimeLeft = 0.f;
	float RepathTimeLeft = 0.f;
	float WaitTimeLeft = 0.f;
	float SearchTimeLeft = 0.f;
};
