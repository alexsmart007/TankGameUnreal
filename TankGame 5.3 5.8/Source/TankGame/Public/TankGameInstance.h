// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TankGameInstance.generated.h"

/**
 * Progress that has to survive level loads (the static fields of Unity's GenerateLevel).
 */
UCLASS()
class TANKGAME_API UTankGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// Reset level and lives to start a new game
	UFUNCTION(BlueprintCallable, Category = "Tank Game")
	void ResetProgress();

	// Move on to the next level; called each time a level loads
	void AdvanceLevel() { ++Level; }

	// Lose a life; returns the lives left
	int32 LoseLife();

	UFUNCTION(BlueprintPure, Category = "Tank Game")
	int32 GetLevelNumber() const { return Level; }

	UFUNCTION(BlueprintPure, Category = "Tank Game")
	int32 GetLives() const { return Lives; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tank Game")
	int32 StartingLives = 3;

private:
	int32 Level = 0;
	int32 Lives = 0;
};
