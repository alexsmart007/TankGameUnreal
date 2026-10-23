// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTank.h"
#include "MyInputConfigData.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerTank::APlayerTank()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	// Overhead camera that follows the tank but does not turn with it
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 1500.f;
	CameraBoom->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	EngineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
	EngineAudio->SetupAttachment(RootComponent);
	EngineAudio->bAutoActivate = false;
}

void APlayerTank::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();
}

void APlayerTank::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputMapping)
			{
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}
}

void APlayerTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC || !InputActions)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Input Actions data asset is not set, controls are disabled."), *GetName());
		return;
	}

	// Completed fires with a zero value, which stops the tank when the key is released
	if (InputActions->InputMove)
	{
		EIC->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APlayerTank::OnMoveInput);
		EIC->BindAction(InputActions->InputMove, ETriggerEvent::Completed, this, &APlayerTank::OnMoveInput);
	}
	if (InputActions->InputRotate)
	{
		EIC->BindAction(InputActions->InputRotate, ETriggerEvent::Triggered, this, &APlayerTank::OnRotateInput);
		EIC->BindAction(InputActions->InputRotate, ETriggerEvent::Completed, this, &APlayerTank::OnRotateInput);
	}
	if (InputActions->InputShoot)
	{
		EIC->BindAction(InputActions->InputShoot, ETriggerEvent::Started, this, &APlayerTank::OnShootInput);
	}
	if (InputActions->InputRestart)
	{
		EIC->BindAction(InputActions->InputRestart, ETriggerEvent::Started, this, &APlayerTank::OnRestartInput);
	}
	if (InputActions->InputQuit)
	{
		EIC->BindAction(InputActions->InputQuit, ETriggerEvent::Started, this, &APlayerTank::OnQuitInput);
	}
}

void APlayerTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveInput != 0.f)
	{
		MoveForward(MoveInput, DeltaTime);
	}
	if (RotateInput != 0.f)
	{
		Turn(RotateInput, DeltaTime);
	}

	if (EngineAudio->Sound)
	{
		const bool bMoving = MoveInput != 0.f;
		if (bMoving && !EngineAudio->IsPlaying())
		{
			EngineAudio->Play();
		}
		else if (!bMoving && EngineAudio->IsPlaying())
		{
			EngineAudio->Stop();
		}
	}
}

void APlayerTank::OnMoveInput(const FInputActionValue& Value)
{
	MoveInput = Value.Get<float>();
}

void APlayerTank::OnRotateInput(const FInputActionValue& Value)
{
	RotateInput = Value.Get<float>();
}

void APlayerTank::OnShootInput(const FInputActionValue& Value)
{
	Fire();
}

void APlayerTank::OnRestartInput(const FInputActionValue& Value)
{
	Respawn();
}

void APlayerTank::Respawn()
{
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);
}

void APlayerTank::OnQuitInput(const FInputActionValue& Value)
{
	UKismetSystemLibrary::QuitGame(this, Cast<APlayerController>(Controller), EQuitPreference::Quit, false);
}
