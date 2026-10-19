// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameMode.h"
#include "EnemyTank.h"
#include "PlayerTank.h"
#include "TankGameInstance.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ATankGameMode::ATankGameMode()
{
	DefaultPawnClass = APlayerTank::StaticClass();
	EnemyTankClass = AEnemyTank::StaticClass();
}

void ATankGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UTankGameInstance* GI = GetTankGameInstance())
	{
		GI->Level++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TankGameMode: set Project Settings > Maps & Modes > Game Instance Class to TankGameInstance, or lives and levels will not work."));
	}

	SpawnEnemies();

	OnLevelStarted(GetGameLevel());
	OnLivesChanged(GetLives());
}

void ATankGameMode::SpawnEnemies()
{
	// Collect spawn points by number from tags like EnemySpawn1, EnemySpawn2, ...
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

	if (EnemyTankClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		for (int32 i = 1; i <= GetGameLevel(); ++i)
		{
			const AActor* const* SpawnPoint = SpawnPoints.Find(i);
			if (!SpawnPoint)
			{
				UE_LOG(LogTemp, Warning, TEXT("TankGameMode: level %d needs a spawn point tagged '%s%d', but none was found."),
					GetGameLevel(), *EnemySpawnTagPrefix, i);
				continue;
			}
			GetWorld()->SpawnActor<AEnemyTank>(EnemyTankClass, (*SpawnPoint)->GetActorLocation(), (*SpawnPoint)->GetActorRotation(), Params);
		}
	}

	// Count every enemy, including ones placed by hand in the level
	EnemiesRemaining = 0;
	for (TActorIterator<AEnemyTank> It(GetWorld()); It; ++It)
	{
		++EnemiesRemaining;
	}

	if (EnemiesRemaining == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TankGameMode: no enemy tanks in the level, so it can never be completed."));
	}
}

void ATankGameMode::TankDestroyed(ATankPawn* Tank)
{
	if (!Tank)
	{
		return;
	}

	if (Tank->IsA<APlayerTank>())
	{
		APlayerController* PC = Cast<APlayerController>(Tank->GetController());
		Tank->Destroy();

		UTankGameInstance* GI = GetTankGameInstance();
		const int32 Lives = GI ? --GI->Lives : 0;
		OnLivesChanged(Lives);

		if (Lives <= 0)
		{
			bRoundOver = true;
			UGameplayStatics::PlaySound2D(this, GameOverSound);
			OnGameOver();
		}
		else if (PC)
		{
			// Spawns a new player tank at the PlayerStart
			RestartPlayer(PC);
		}
		return;
	}

	Tank->Destroy();
	if (bRoundOver || --EnemiesRemaining > 0)
	{
		return;
	}

	bRoundOver = true;
	if (GetGameLevel() >= FinalLevel)
	{
		UGameplayStatics::PlaySound2D(this, WinSound);
		OnWin();
	}
	else
	{
		UGameplayStatics::PlaySound2D(this, LevelCompleteSound);
		OnLevelComplete(GetGameLevel());
		GetWorldTimerManager().SetTimer(NextLevelTimer, this, &ATankGameMode::LoadNextLevel, TransitionTime);
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

int32 ATankGameMode::GetGameLevel() const
{
	const UTankGameInstance* GI = GetTankGameInstance();
	return GI ? GI->Level : 1;
}

int32 ATankGameMode::GetLives() const
{
	const UTankGameInstance* GI = GetTankGameInstance();
	return GI ? GI->Lives : 0;
}

UTankGameInstance* ATankGameMode::GetTankGameInstance() const
{
	return Cast<UTankGameInstance>(GetGameInstance());
}
