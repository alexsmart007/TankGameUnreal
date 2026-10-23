// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAIController.h"
#include "EnemyTank.h"
#include "PlayerTank.h"
#include "TankGameMode.h"
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
	if (!Tank)
	{
		return;
	}

	// Use this tank's view settings
	SightConfig->SightRadius = Tank->SightDistance;
	SightConfig->LoseSightRadius = Tank->SightDistance + 500.f;
	SightConfig->PeripheralVisionAngleDegrees = Tank->SightAngle;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ATankAIController::HandleTargetPerceptionUpdated);

	State = ETankAIState::Roam;
	WaitTimeLeft = FMath::FRandRange(Tank->RoamPause.X, Tank->RoamPause.Y);
}

void ATankAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APlayerTank* Player = Cast<APlayerTank>(Actor);
	if (!Player)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		Target = Player;
		bCanSeeTarget = true;
		LastKnownLocation = Player->GetActorLocation();
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

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Tank)
	{
		return;
	}

	// The player was destroyed while in sight
	if (bCanSeeTarget && !Target.IsValid())
	{
		bCanSeeTarget = false;
		SetAIState(ETankAIState::Roam);
	}
	if (bCanSeeTarget)
	{
		LastKnownLocation = Target->GetActorLocation();
	}

	switch (State)
	{
	case ETankAIState::Roam:   TickRoam(DeltaTime);   break;
	case ETankAIState::Chase:  TickChase(DeltaTime);  break;
	case ETankAIState::Attack: TickAttack(DeltaTime); break;
	case ETankAIState::Search: TickSearch(DeltaTime); break;
	}

	if (Tank->bDrawDebug)
	{
		DrawDebug();
	}
}

void ATankAIController::SetAIState(ETankAIState NewState)
{
	if (State == NewState)
	{
		return;
	}

	State = NewState;
	PathPoints.Reset();
	WaitTimeLeft = 0.f;
	RepathTimeLeft = 0.f;

	if (NewState == ETankAIState::Search)
	{
		SearchTimeLeft = Tank->SearchDuration;
		if (Tank->bCanRoam)
		{
			SetDestination(LastKnownLocation);
		}
	}
}

void ATankAIController::TickRoam(float DeltaTime)
{
	if (!Tank->bCanRoam)
	{
		// Stationary turret: spin in place until the player walks into view
		Tank->AddActorWorldRotation(FRotator(0.f, Tank->ScanSpeed * DeltaTime, 0.f));
		return;
	}

	if (WaitTimeLeft > 0.f)
	{
		WaitTimeLeft -= DeltaTime;
		return;
	}

	if (PathPoints.IsEmpty())
	{
		const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		FNavLocation RoamPoint;
		if (!NavSystem || !NavSystem->GetRandomReachablePointInRadius(Tank->GetActorLocation(), Tank->RoamRadius, RoamPoint)
			|| !SetDestination(RoamPoint.Location))
		{
			// No nav mesh here; try again shortly
			WaitTimeLeft = 1.f;
		}
		return;
	}

	if (FollowPath(DeltaTime))
	{
		WaitTimeLeft = FMath::FRandRange(Tank->RoamPause.X, Tank->RoamPause.Y);
	}
}

void ATankAIController::TickChase(float DeltaTime)
{
	if (!bCanSeeTarget)
	{
		SetAIState(ETankAIState::Search);
		return;
	}

	if (!Tank->bCanRoam || FVector::Dist2D(Tank->GetActorLocation(), LastKnownLocation) <= Tank->AttackRange)
	{
		SetAIState(ETankAIState::Attack);
		return;
	}

	RepathTimeLeft -= DeltaTime;
	if (RepathTimeLeft <= 0.f || PathPoints.IsEmpty())
	{
		SetDestination(LastKnownLocation);
		RepathTimeLeft = ChaseRepathInterval;
	}
	FollowPath(DeltaTime);
}

void ATankAIController::TickAttack(float DeltaTime)
{
	if (!bCanSeeTarget)
	{
		SetAIState(ETankAIState::Search);
		return;
	}

	const FVector ToTarget = LastKnownLocation - Tank->GetActorLocation();

	// Some slack before chasing again, so the tank doesn't flip between the two states at the edge of its range
	if (Tank->bCanRoam && ToTarget.Size2D() > Tank->AttackRange * 1.2f)
	{
		SetAIState(ETankAIState::Chase);
		return;
	}

	const float AngleLeft = Tank->TurnTowards(ToTarget, DeltaTime);

	const ATankGameMode* GameMode = GetWorld()->GetAuthGameMode<ATankGameMode>();
	const bool bRoundActive = !GameMode || GameMode->IsRoundActive();
	if (AngleLeft <= Tank->AimTolerance && bRoundActive && Tank->CanFire() && HasClearShot())
	{
		Tank->Fire();
	}
}

void ATankAIController::TickSearch(float DeltaTime)
{
	// Drive to where the player was last seen, then look around
	if (!PathPoints.IsEmpty())
	{
		FollowPath(DeltaTime);
		return;
	}

	Tank->AddActorWorldRotation(FRotator(0.f, Tank->ScanSpeed * 2.f * DeltaTime, 0.f));
	SearchTimeLeft -= DeltaTime;
	if (SearchTimeLeft <= 0.f)
	{
		SetAIState(ETankAIState::Roam);
	}
}

bool ATankAIController::SetDestination(const FVector& Destination)
{
	PathPoints.Reset();
	PathIndex = 0;

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, Tank->GetActorLocation(), Destination, Tank);
	if (!Path || !Path->IsValid() || Path->PathPoints.Num() < 2)
	{
		return false;
	}

	// The first point is where the tank already is
	PathPoints = Path->PathPoints;
	PathIndex = 1;

	// Give up on the path if it takes much longer than expected (stuck on another tank, for example)
	PathTimeLeft = Path->GetPathLength() / FMath::Max(Tank->GetMoveSpeed(), 1.f) * 2.f + 2.f;
	return true;
}

bool ATankAIController::FollowPath(float DeltaTime)
{
	PathTimeLeft -= DeltaTime;
	if (!PathPoints.IsValidIndex(PathIndex) || PathTimeLeft <= 0.f)
	{
		PathPoints.Reset();
		return true;
	}

	FVector ToPoint = PathPoints[PathIndex] - Tank->GetActorLocation();
	ToPoint.Z = 0.f;
	if (ToPoint.Size() < AcceptanceRadius)
	{
		if (!PathPoints.IsValidIndex(++PathIndex))
		{
			PathPoints.Reset();
			return true;
		}
		return false;
	}

	// Turn first, then drive, so the tank never slides sideways
	if (Tank->TurnTowards(ToPoint, DeltaTime) < DriveAngle)
	{
		Tank->MoveForward(1.f, DeltaTime);
	}
	return false;
}

bool ATankAIController::HasClearShot() const
{
	if (!Target.IsValid())
	{
		return false;
	}

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
	if (!GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectTypes, Params))
	{
		return true;
	}
	return Hit.GetActor() == Target.Get();
}

void ATankAIController::DrawDebug() const
{
	const UWorld* World = GetWorld();
	const FVector Location = Tank->GetActorLocation();

	static const FColor StateColors[] = { FColor::Green, FColor::Orange, FColor::Red, FColor::Yellow };
	const FColor Color = StateColors[static_cast<uint8>(State)];

	DrawDebugString(World, Location + FVector(0.f, 0.f, 150.f), UEnum::GetDisplayValueAsText(State).ToString(), nullptr, Color, 0.f, true, 1.5f);

	// View cone edges
	for (const float Side : { -1.f, 1.f })
	{
		const FVector Edge = Tank->GetActorForwardVector().RotateAngleAxis(Side * Tank->SightAngle, FVector::UpVector);
		DrawDebugLine(World, Location, Location + Edge * Tank->SightDistance, Color);
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
