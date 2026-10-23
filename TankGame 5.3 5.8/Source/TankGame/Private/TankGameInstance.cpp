// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameInstance.h"

void UTankGameInstance::Init()
{
	Super::Init();
	ResetProgress();
}

void UTankGameInstance::ResetProgress()
{
	Level = 0;
	Lives = StartingLives;
}

int32 UTankGameInstance::LoseLife()
{
	Lives = FMath::Max(0, Lives - 1);
	return Lives;
}
