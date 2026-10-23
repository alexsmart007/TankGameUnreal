// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

/**
 * Player controller that owns the cursor and input mode.
 */
UCLASS()
class TANKGAME_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Show the mouse so the Game Over / Win buttons can be clicked; keyboard input keeps working
	UFUNCTION(BlueprintCallable, Category = "Tank Game")
	void ShowMenuCursor();
};
