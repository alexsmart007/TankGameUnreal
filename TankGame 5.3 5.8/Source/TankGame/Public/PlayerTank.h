// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "InputActionValue.h"
#include "PlayerTank.generated.h"

class UAudioComponent;
class UCameraComponent;
class UInputMappingContext;
class UMyInputConfigData;
class USpringArmComponent;

/**
 * Player tank: Unity's InputManager, MovementControl, RotateControl, Respawner and QuitHandler.
 */
UCLASS()
class TANKGAME_API APlayerTank : public ATankPawn
{
	GENERATED_BODY()

public:
	APlayerTank();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

	// Move back to where the tank started (Unity's Respawner, bound to Restart)
	UFUNCTION(BlueprintCallable, Category = "Tank")
	void Respawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	// Loops while the tank drives; set its Sound in the Blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> EngineAudio;

#pragma region /** Input */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UMyInputConfigData> InputActions;
#pragma endregion

	// Unity speed 2.5 (m/s) = 250 cm/s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Movement")
	float MoveSpeed = 250.f;

	// Degrees per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Movement")
	float RotateSpeed = 100.f;

private:
	void OnMoveInput(const FInputActionValue& Value);
	void OnRotateInput(const FInputActionValue& Value);
	void OnShootInput(const FInputActionValue& Value);
	void OnRestartInput(const FInputActionValue& Value);
	void OnQuitInput(const FInputActionValue& Value);

	float MoveInput = 0.f;
	float RotateInput = 0.f;
	FTransform InitialTransform;
};
