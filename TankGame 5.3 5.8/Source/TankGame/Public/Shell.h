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
 * Bounces off a wall once and explodes on the second wall. It destroys any tank it hits; the tank
 * that fired it is only hittable after the first bounce. Two shells that collide both explode.
 */
UCLASS()
class TANKGAME_API AShell : public AActor
{
	GENERATED_BODY()

public:
	AShell();

	// Call before FinishSpawning
	void SetSpeed(float Speed);

	bool HasBounced() const { return bBounced; }
	bool HasExploded() const { return bExploded; }

protected:
	virtual void BeginPlay() override;

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

	// First wall hit: keep flying, and from now on the shooter can be hit too
	void Bounce();

	void Explode();

	bool bBounced = false;
	bool bExploded = false;
};
