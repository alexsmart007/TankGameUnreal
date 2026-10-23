// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAIController.h"
#include "EnemyTank.h"
#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

namespace
{
	// How close the tank must get to a path point before heading for the next one
	constexpr float AcceptanceRadius = 75.f;

	// The tank only drives forward once it is facing within this angle of the next path point
	constexpr float DriveAngle = 30.f;

	// How often a chase recalculates its path to the player
	constexpr float ChaseRepathInterval = 0.5f;

	// Moving less than this fraction of full speed counts as blocked
	constexpr float StuckSpeedFraction = 0.25f;

	// Blocked for this long means stuck
	constexpr float StuckThreshold = 0.5f;

	// How long to back up when stuck
	constexpr float UnstuckDuration = 0.8f;
}

ATankAIController::ATankAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = 3000.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	// There are no teams, so detect every pawn and filter for the player in the handler
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	UAIPerceptionComponent* Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*Perception);
}

void ATankAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Tank = Cast<AEnemyTank>(InPawn);
	if (Tank)
	{
		const FTankAIConfig& Config = Tank->GetAIConfig();

		// Use this tank's view settings
		SightConfig->SightRadius = Config.SightDistance;
		SightConfig->LoseSightRadius = Config.SightDistance + 500.f;
		SightConfig->PeripheralVisionAngleDegrees = Config.SightAngle;
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ATankAIController::HandleTargetPerceptionUpdated);

		Tank->OnRoundReset.AddUObject(this, &ATankAIController::HandleRoundReset);
		HandleRoundReset();
	}
}

void ATankAIController::OnUnPossess()
{
	if (Tank)
	{
		Tank->OnRoundReset.RemoveAll(this);
		Tank = nullptr;
	}
	Super::OnUnPossess();
}

void ATankAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* SeenPawn = Cast<APawn>(Actor);
	if (SeenPawn && SeenPawn->IsPlayerControlled())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			Target = SeenPawn;
			bCanSeeTarget = true;
			LastKnownLocation = SeenPawn->GetActorLocation();
			if (State == ETankAIState::Roam || State == ETankAIState::Search)
			{
				SetAIState(ETankAIState::Chase);
			}
		}
		else
		{
			bCanSeeTarget = false;
			LastKnownLocation = Stimulus.StimulusLocation;
			if (State == ETankAIState::Chase || State == ETankAIState::Attack)
			{
				SetAIState(ETankAIState::Search);
			}
		}
	}
}

void ATankAIController::HandleRoundReset()
{
	Target.Reset();
	bCanSeeTarget = false;
	State = ETankAIState::Roam;
	PathPoints.Reset();
	StuckTime = 0.f;
	UnstuckTimeLeft = 0.f;
	WaitTimeLeft = FMath::FRandRange(Tank->GetAIConfig().RoamPause.X, Tank->GetAIConfig().RoamPause.Y);
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Tank)
	{
		if (bCanSeeTarget && !Target.IsValid())
		{
			// The player was destroyed while in sight
			bCanSeeTarget = false;
			SetAIState(ETankAIState::Roam);
		}
		else if (bCanSeeTarget)
		{
			LastKnownLocation = Target->GetActorLocation();
		}

		if (UnstuckTimeLeft > 0.f)
		{
			TickUnstuck(DeltaTime);
		}
		else
		{
			TickState(DeltaTime);
		}

		if (Tank->GetAIConfig().bDrawDebug)
		{
			DrawDebug();
		}
	}
}

void ATankAIController::SetAIState(ETankAIState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		PathPoints.Reset();
		WaitTimeLeft = 0.f;
		RepathTimeLeft = 0.f;
		StuckTime = 0.f;
		UnstuckTimeLeft = 0.f;

		if (NewState == ETankAIState::Search)
		{
			SearchTimeLeft = Tank->GetAIConfig().SearchDuration;
			if (Tank->GetAIConfig().bCanRoam)
			{
				SetDestination(LastKnownLocation);
			}
		}
	}
}

void ATankAIController::TickState(float DeltaTime)
{
	switch (State)
	{
	case ETankAIState::Roam:   TickRoam(DeltaTime);   break;
	case ETankAIState::Chase:  TickChase(DeltaTime);  break;
	case ETankAIState::Attack: TickAttack(DeltaTime); break;
	case ETankAIState::Search: TickSearch(DeltaTime); break;
	}
}

void ATankAIController::TickRoam(float DeltaTime)
{
	const FTankAIConfig& Config = Tank->GetAIConfig();

	if (!Config.bCanRoam)
	{
		// Stationary turret: spin in place until the player comes into view
		Tank->AddActorWorldRotation(FRotator(0.f, Config.ScanSpeed * DeltaTime, 0.f));
	}
	else if (WaitTimeLeft > 0.f)
	{
		WaitTimeLeft -= DeltaTime;
	}
	else if (PathPoints.IsEmpty())
	{
		if (!SetRandomRoamDestination())
		{
			// No nav mesh here; try again shortly
			WaitTimeLeft = 1.f;
		}
	}
	else if (FollowPath(DeltaTime))
	{
		WaitTimeLeft = FMath::FRandRange(Config.RoamPause.X, Config.RoamPause.Y);
	}
}

void ATankAIController::TickChase(float DeltaTime)
{
	const FTankAIConfig& Config = Tank->GetAIConfig();

	if (!bCanSeeTarget)
	{
		SetAIState(ETankAIState::Search);
	}
	else if (!Config.bCanRoam || FVector::Dist2D(Tank->GetActorLocation(), LastKnownLocation) <= Config.AttackRange)
	{
		SetAIState(ETankAIState::Attack);
	}
	else
	{
		RepathTimeLeft -= DeltaTime;
		if (RepathTimeLeft <= 0.f || PathPoints.IsEmpty())
		{
			SetDestination(LastKnownLocation);
			RepathTimeLeft = ChaseRepathInterval;
		}
		FollowPath(DeltaTime);
	}
}

void ATankAIController::TickAttack(float DeltaTime)
{
	const FTankAIConfig& Config = Tank->GetAIConfig();
	const FVector ToTarget = LastKnownLocation - Tank->GetActorLocation();

	if (!bCanSeeTarget)
	{
		SetAIState(ETankAIState::Search);
	}
	else if (Config.bCanRoam && ToTarget.Size2D() > Config.AttackRange * 1.2f)
	{
		// Some slack before chasing again, so the tank doesn't flip between the two states at the edge of its range
		SetAIState(ETankAIState::Chase);
	}
	else
	{
		const float AngleLeft = Tank->TurnTowards(ToTarget, DeltaTime);
		if (AngleLeft <= Config.AimTolerance && Tank->CanFire() && HasClearShot())
		{
			Tank->Fire();
		}
	}
}

void ATankAIController::TickSearch(float DeltaTime)
{
	if (!PathPoints.IsEmpty())
	{
		// Drive to where the player was last seen
		FollowPath(DeltaTime);
	}
	else
	{
		// Then look around
		Tank->AddActorWorldRotation(FRotator(0.f, Tank->GetAIConfig().ScanSpeed * 2.f * DeltaTime, 0.f));
		SearchTimeLeft -= DeltaTime;
		if (SearchTimeLeft <= 0.f)
		{
			SetAIState(ETankAIState::Roam);
		}
	}
}

void ATankAIController::TickUnstuck(float DeltaTime)
{
	UnstuckTimeLeft -= DeltaTime;
	Tank->MoveForward(-0.6f, DeltaTime);
	Tank->Turn(UnstuckTurn, DeltaTime);
}

void ATankAIController::StartUnstuck()
{
	StuckTime = 0.f;
	UnstuckTimeLeft = UnstuckDuration;
	UnstuckTurn = FMath::RandBool() ? 1.f : -1.f;

	// Plan a fresh path once free
	PathPoints.Reset();
	RepathTimeLeft = 0.f;
}

bool ATankAIController::SetRandomRoamDestination()
{
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation RoamPoint;
	return NavSystem
		&& NavSystem->GetRandomReachablePointInRadius(Tank->GetActorLocation(), Tank->GetAIConfig().RoamRadius, RoamPoint)
		&& SetDestination(RoamPoint.Location);
}

bool ATankAIController::SetDestination(const FVector& Destination)
{
	PathPoints.Reset();
	PathIndex = 0;

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, Tank->GetActorLocation(), Destination, Tank);
	const bool bFound = Path && Path->IsValid() && Path->PathPoints.Num() >= 2;
	if (bFound)
	{
		// The first point is where the tank already is
		PathPoints = Path->PathPoints;
		PathIndex = 1;

		// Give up on the path if it takes much longer than expected
		PathTimeLeft = Path->GetPathLength() / FMath::Max(Tank->GetMoveSpeed(), 1.f) * 2.f + 2.f;
	}
	return bFound;
}

bool ATankAIController::FollowPath(float DeltaTime)
{
	bool bFinished = false;
	PathTimeLeft -= DeltaTime;

	if (!PathPoints.IsValidIndex(PathIndex) || PathTimeLeft <= 0.f)
	{
		PathPoints.Reset();
		bFinished = true;
	}
	else
	{
		FVector ToPoint = PathPoints[PathIndex] - Tank->GetActorLocation();
		ToPoint.Z = 0.f;

		if (ToPoint.Size() < AcceptanceRadius)
		{
			++PathIndex;
			if (!PathPoints.IsValidIndex(PathIndex))
			{
				PathPoints.Reset();
				bFinished = true;
			}
		}
		// Turn first, then drive, so the tank never slides sideways
		else if (Tank->TurnTowards(ToPoint, DeltaTime) < DriveAngle)
		{
			const FVector Before = Tank->GetActorLocation();
			Tank->MoveForward(1.f, DeltaTime);

			const float Moved = FVector::Dist2D(Before, Tank->GetActorLocation());
			StuckTime = Moved < Tank->GetMoveSpeed() * DeltaTime * StuckSpeedFraction ? StuckTime + DeltaTime : 0.f;
			if (StuckTime >= StuckThreshold)
			{
				StartUnstuck();
				bFinished = true;
			}
		}
	}

	return bFinished;
}

bool ATankAIController::HasClearShot() const
{
	bool bClear = false;

	if (Target.IsValid())
	{
		// Shells fly level at the fire point's height, so test along that same line
		const FVector Start = Tank->GetFirePointLocation();
		FVector End = Target->GetActorLocation();
		End.Z = Start.Z;

		FCollisionObjectQueryParams ObjectTypes;
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
		const FCollisionQueryParams Params(SCENE_QUERY_STAT(TankAIClearShot), false, Tank);

		FHitResult Hit;
		const bool bBlocked = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectTypes, Params);
		bClear = !bBlocked || Hit.GetActor() == Target.Get();
	}

	return bClear;
}

void ATankAIController::DrawDebug() const
{
	const UWorld* World = GetWorld();
	const FTankAIConfig& Config = Tank->GetAIConfig();
	const FVector Location = Tank->GetActorLocation();

	static const FColor StateColors[] = { FColor::Green, FColor::Orange, FColor::Red, FColor::Yellow };
	const FColor Color = StateColors[static_cast<uint8>(State)];

	DrawDebugString(World, Location + FVector(0.f, 0.f, 150.f), UEnum::GetDisplayValueAsText(State).ToString(), nullptr, Color, 0.f, true, 1.5f);

	// View cone edges
	for (const float Side : { -1.f, 1.f })
	{
		const FVector Edge = Tank->GetActorForwardVector().RotateAngleAxis(Side * Config.SightAngle, FVector::UpVector);
		DrawDebugLine(World, Location, Location + Edge * Config.SightDistance, Color);
	}

	// Remaining path
	FVector From = Location;
	for (int32 i = PathIndex; i < PathPoints.Num(); ++i)
	{
		DrawDebugLine(World, From, PathPoints[i], FColor::Cyan, false, -1.f, 0, 3.f);
		DrawDebugSphere(World, PathPoints[i], 20.f, 8, FColor::Cyan);
		From = PathPoints[i];
	}

	if (State == ETankAIState::Search)
	{
		DrawDebugSphere(World, LastKnownLocation, 50.f, 12, FColor::Yellow);
	}
}
