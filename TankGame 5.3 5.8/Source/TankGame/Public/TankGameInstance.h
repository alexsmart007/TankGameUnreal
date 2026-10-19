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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tank Game")
	int32 StartingLives = 3;

	// Current level, incremented by ATankGameMode every time a level loads
	UPROPERTY(BlueprintReadOnly, Category = "Tank Game")
	int32 Level = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Tank Game")
	int32 Lives = 3;
};
