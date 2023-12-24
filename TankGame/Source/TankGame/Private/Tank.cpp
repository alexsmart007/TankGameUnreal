// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "MyInputConfigData.h"
#include "InputActionValue.h"

// Sets default values
ATank::ATank()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Get the player controller
    APlayerController* PC = Cast<APlayerController>(GetController());

    // Get the local player subsystem
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
    // Clear out existing mapping, and add our mapping
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(InputMapping, 0);

    // Get the EnhancedInputComponent
    UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    // Bind the actions
    PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ATank::Move);
    PEI->BindAction(InputActions->InputShoot, ETriggerEvent::Triggered, this, &ATank::Shoot);
    PEI->BindAction(InputActions->InputRotate, ETriggerEvent::Triggered, this, &ATank::Rotate);
}

void ATank::Move(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        const FVector2D MoveValue = Value.Get<FVector2D>();
        const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

        // Forward/Backward direction
        if (MoveValue.Y != 0.f)
        {
            // Get forward vector
            const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);

            AddMovementInput(Direction, MoveValue.Y);
        }

        // Right/Left direction
        if (MoveValue.X != 0.f)
        {
            // Get right vector
            const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);

            AddMovementInput(Direction, MoveValue.X);
        }
    }
}

void ATank::Rotate(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        const FVector2D LookValue = Value.Get<FVector2D>();

        if (LookValue.X != 0.f)
        {
            AddControllerYawInput(LookValue.X);
        }

        if (LookValue.Y != 0.f)
        {
            AddControllerPitchInput(LookValue.Y);
        }
    }
}

void ATank::Shoot(const FInputActionValue& Value)
{
    printf("Shoot");
}

