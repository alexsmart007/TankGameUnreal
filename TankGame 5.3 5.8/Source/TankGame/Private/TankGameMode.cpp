// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameMode.h"
#include "EnemyTank.h"
#include "PlayerTank.h"
#include "TankGameInstance.h"
#include "TankPlayerController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ATankGameMode::ATankGameMode()
{
	DefaultPawnClass = APlayerTank::StaticClass();
	PlayerControllerClass = ATankPlayerController::StaticClass();
	EnemyTankClass = AEnemyTank::StaticClass();
}

void ATankGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UTankGameInstance* GI = GetTankGameInstance())
	{
		GI->AdvanceLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TankGameMode: set Project Settings > Maps & Modes > Game Instance Class to TankGameInstance, or lives and levels will not work."));
	}

	SpawnEnemies();

	OnLevelStarted(GetGameLevel());
	OnLivesChanged(GetLives());
}

void ATankGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);
	WatchTank(Cast<ATankPawn>(PlayerPawn));
}

#pragma region Level setup

void ATankGameMode::SpawnEnemies()
{
	const TMap<int32, AActor*> SpawnPoints = FindEnemySpawnPoints();

	if (EnemyTankClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		for (int32 i = 1; i <= GetGameLevel(); ++i)
		{
			if (const AActor* const* SpawnPoint = SpawnPoints.Find(i))
			{
				GetWorld()->SpawnActor<AEnemyTank>(EnemyTankClass, (*SpawnPoint)->GetActorLocation(), (*SpawnPoint)->GetActorRotation(), Params);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TankGameMode: level %d needs a spawn point tagged '%s%d', but none was found."),
					GetGameLevel(), *EnemySpawnTagPrefix, i);
			}
		}
	}

	// Watch and count every enemy, including ones placed by hand in the level
	EnemiesRemaining = 0;
	for (TActorIterator<AEnemyTank> It(GetWorld()); It; ++It)
	{
		WatchTank(*It);
		++EnemiesRemaining;
	}

	if (EnemiesRemaining == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TankGameMode: no enemy tanks in the level, so it can never be completed."));
	}
}

TMap<int32, AActor*> ATankGameMode::FindEnemySpawnPoints() const
{
	TMap<int32, AActor*> SpawnPoints;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		for (const FName& Tag : It->Tags)
		{
			const FString TagString = Tag.ToString();
			const FString Number = TagString.RightChop(EnemySpawnTagPrefix.Len());
			if (TagString.StartsWith(EnemySpawnTagPrefix) && !Number.IsEmpty() && Number.IsNumeric())
			{
				SpawnPoints.Add(FCString::Atoi(*Number), *It);
			}
		}
	}
	return SpawnPoints;
}

void ATankGameMode::WatchTank(ATankPawn* Tank)
{
	if (Tank && !Tank->OnTankDestroyed.IsBoundToObject(this))
	{
		Tank->OnTankDestroyed.AddUObject(this, &ATankGameMode::HandleTankDestroyed);
	}
}

#pragma endregion

#pragma region Round flow

void ATankGameMode::HandleTankDestroyed(ATankPawn* Tank)
{
	if (Tank->IsPlayerControlled())
	{
		HandlePlayerDestroyed(Tank->GetController());
	}
	else
	{
		HandleEnemyDestroyed();
	}
}

void ATankGameMode::HandlePlayerDestroyed(AController* PlayerController)
{
	UTankGameInstance* GI = GetTankGameInstance();
	const int32 Lives = GI ? GI->LoseLife() : 0;
	OnLivesChanged(Lives);

	if (Lives <= 0)
	{
		EndRound();
		UGameplayStatics::PlaySound2D(this, GameOverSound);
		ShowMenuCursor();
		OnGameOver();
	}
	else
	{
		// Pause so the explosion is seen, then reset (also keeps the reset out of the shell's hit callback)
		GetWorldTimerManager().SetTimer(RespawnTimer,
			FTimerDelegate::CreateUObject(this, &ATankGameMode::ResetRound, TWeakObjectPtr<AController>(PlayerController)),
			FMath::Max(RespawnDelay, 0.01f), false);
	}
}

void ATankGameMode::HandleEnemyDestroyed()
{
	if (!bRoundOver)
	{
		--EnemiesRemaining;
		if (EnemiesRemaining <= 0)
		{
			CompleteLevel();
		}
	}
}

void ATankGameMode::ResetRound(TWeakObjectPtr<AController> PlayerController)
{
	if (!bRoundOver)
	{
		// Surviving enemies go back to where they started; destroyed ones stay destroyed
		for (TActorIterator<ATankPawn> It(GetWorld()); It; ++It)
		{
			It->ResetForNewRound();
		}

		if (PlayerController.IsValid())
		{
			// Spawns a new player tank at the PlayerStart
			RestartPlayer(PlayerController.Get());
		}
	}
}

void ATankGameMode::EndRound()
{
	bRoundOver = true;
	for (TActorIterator<ATankPawn> It(GetWorld()); It; ++It)
	{
		It->SetFiringEnabled(false);
	}
}

void ATankGameMode::CompleteLevel()
{
	EndRound();

	if (GetGameLevel() >= FinalLevel)
	{
		UGameplayStatics::PlaySound2D(this, WinSound);
		ShowMenuCursor();
		OnWin();
	}
	else
	{
		UGameplayStatics::PlaySound2D(this, LevelCompleteSound);
		OnLevelComplete(GetGameLevel());
		GetWorldTimerManager().SetTimer(NextLevelTimer, this, &ATankGameMode::LoadNextLevel, TransitionTime);
	}
}

void ATankGameMode::ShowMenuCursor()
{
	if (ATankPlayerController* PC = Cast<ATankPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->ShowMenuCursor();
	}
}

void ATankGameMode::LoadNextLevel()
{
	// GetGameLevel() is the level just finished, so it is also the index of the next map
	OpenMapForLevel(GetGameLevel());
}

void ATankGameMode::RestartGame()
{
	if (UTankGameInstance* GI = GetTankGameInstance())
	{
		GI->ResetProgress();
	}
	OpenMapForLevel(0);
}

void ATankGameMode::OpenMapForLevel(int32 LevelIndex)
{
	const FName Map = LevelMaps.IsValidIndex(LevelIndex) && !LevelMaps[LevelIndex].IsNone()
		? LevelMaps[LevelIndex]
		: FName(*UGameplayStatics::GetCurrentLevelName(this));
	UGameplayStatics::OpenLevel(this, Map);
}

#pragma endregion

int32 ATankGameMode::GetGameLevel() const
{
	const UTankGameInstance* GI = GetTankGameInstance();
	return GI ? GI->GetLevelNumber() : 1;
}

int32 ATankGameMode::GetLives() const
{
	const UTankGameInstance* GI = GetTankGameInstance();
	return GI ? GI->GetLives() : 0;
}

UTankGameInstance* ATankGameMode::GetTankGameInstance() const
{
	return Cast<UTankGameInstance>(GetGameInstance());
}
