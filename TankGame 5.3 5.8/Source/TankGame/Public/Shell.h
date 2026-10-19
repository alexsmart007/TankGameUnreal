// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shell.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;

/**
 * Tank shell (Unity's ShellExplosionPlayer / ShellExplosionEnemy).
 * Bounces off a wall once and explodes on the second wall. It always destroys a tank of the
 * other side, and destroys a tank of its own side (including the shooter) only after bouncing.
 */
UCLASS()
class TANKGAME_API AShell : public AActor
{
	GENERATED_BODY()

public:
	AShell();

	// Call before FinishSpawning
	void SetSpeed(float Speed);

	bool IsFiredByPlayer() const { return bFiredByPlayer; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shell|Effects")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shell|Effects")
	TObjectPtr<USoundBase> ExplodeSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shell|Effects")
	TObjectPtr<USoundBase> BounceSound;

private:
	UFUNCTION()
	void HandleBounce(const FHitResult& Hit, const FVector& ImpactVelocity);

	void Explode();

	bool bFiredByPlayer = false;
	bool bBounced = false;
	bool bExploded = false;
};
