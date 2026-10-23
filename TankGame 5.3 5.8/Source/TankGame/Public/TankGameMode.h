// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameMode.generated.h"

class AEnemyTank;
class ATankPawn;
class UTankGameInstance;
class USoundBase;

/**
 * Level flow from the Unity project: GenerateLevel (enemy spawning), GameOver (lives),
 * LevelLoader (next level) and Win. Listens to each tank's OnTankDestroyed event.
 */
UCLASS()
class TANKGAME_API ATankGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATankGameMode();

	UFUNCTION(BlueprintPure, Category = "Tank Game")
	int32 GetGameLevel() const;

	UFUNCTION(BlueprintPure, Category = "Tank Game")
	int32 GetLives() const;

	UFUNCTION(BlueprintPure, Category = "Tank Game")
	int32 GetEnemiesRemaining() const { return EnemiesRemaining; }

	// Reset lives and level, then load the first level
	UFUNCTION(BlueprintCallable, Category = "Tank Game")
	void RestartGame();

protected:
	virtual void BeginPlay() override;

	// Called each time a player tank spawns
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

#pragma region /** UI hooks, implement these in the Blueprint subclass */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tank Game")
	void OnLevelStarted(int32 Level);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tank Game")
	void OnLivesChanged(int32 Lives);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tank Game")
	void OnLevelComplete(int32 Level);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tank Game")
	void OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tank Game")
	void OnWin();
#pragma endregion

	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Enemies")
	TSubclassOf<AEnemyTank> EnemyTankClass;

	// Spawn points are tagged with this prefix plus a number (EnemySpawn1, EnemySpawn2, ...); level N uses 1 through N
	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Enemies")
	FString EnemySpawnTagPrefix = TEXT("EnemySpawn");

	// Beating this level wins the game
	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Levels")
	int32 FinalLevel = 11;

	// Pause after the player dies before the round resets and the player respawns
	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Levels")
	float RespawnDelay = 1.f;

	// Delay between clearing a level and loading the next one
	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Levels")
	float TransitionTime = 2.f;

	// Optional map per level (index 0 = level 1). If a level has no entry, the current map is reloaded.
	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Levels")
	TArray<FName> LevelMaps;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Audio")
	TObjectPtr<USoundBase> LevelCompleteSound;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Audio")
	TObjectPtr<USoundBase> GameOverSound;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Game|Audio")
	TObjectPtr<USoundBase> WinSound;

private:
	UTankGameInstance* GetTankGameInstance() const;

#pragma region /** Level setup */
	void SpawnEnemies();

	// Spawn points keyed by their number, from tags like EnemySpawn1, EnemySpawn2, ...
	TMap<int32, AActor*> FindEnemySpawnPoints() const;

	// Start listening for this tank's destruction
	void WatchTank(ATankPawn* Tank);
#pragma endregion

#pragma region /** Round flow */
	void HandleTankDestroyed(ATankPawn* Tank);
	void HandlePlayerDestroyed(AController* PlayerController);
	void HandleEnemyDestroyed();

	// After the player dies: every tank clears its shells and enemies return to their spawn points
	void ResetRound(TWeakObjectPtr<AController> PlayerController);

	// The level is won or lost: stop all firing
	void EndRound();

	void CompleteLevel();
	void ShowMenuCursor();
	void LoadNextLevel();
	void OpenMapForLevel(int32 Level);
#pragma endregion

	int32 EnemiesRemaining = 0;
	bool bRoundOver = false;
	FTimerHandle NextLevelTimer;
	FTimerHandle RespawnTimer;
};
