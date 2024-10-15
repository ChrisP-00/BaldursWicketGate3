// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterCameraActor.h"


AMainCharacterController::AMainCharacterController()
{
	/* Mouse Cursor */
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	/* Destination & Hold and press time */
	TargetDestination = FVector::ZeroVector;
	HoldTime = 0;
}

void AMainCharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn)
	{
		FVector CameraActorLocation = ControlledPawn->GetActorLocation();
		FRotator CameraActorRotation = ControlledPawn->GetActorRotation();
		FActorSpawnParameters SpawnParameters;
		
		CameraActor = GetWorld()->SpawnActor<ACharacterCameraActor>(ACharacterCameraActor::StaticClass(), CameraActorLocation, CameraActorRotation, SpawnParameters);
		if(CameraActor)
		{
			SetViewTarget(CameraActor);
			
			AActor* ControlledActor = Cast<AActor>(ControlledPawn);
			if(ControlledActor)
			{
				CameraActor->SetTarget(ControlledActor);
			}
		}
	}
}

void AMainCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/* Add Input Mapping Context */
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
	}

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		/* Bind Movement Inputs */
		EnhancedInputComponent->BindAction(CharacterMovementControl, ETriggerEvent::Started, this, &AMainCharacterController::OnInputStarted);
		EnhancedInputComponent->BindAction(CharacterMovementControl, ETriggerEvent::Triggered, this, &AMainCharacterController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(CharacterMovementControl, ETriggerEvent::Completed, this, &AMainCharacterController::OnSetDestinationCompleted);
		EnhancedInputComponent->BindAction(CharacterMovementControl, ETriggerEvent::Canceled, this, &AMainCharacterController::OnSetDestinationCompleted);

		/* Bind Camera Inputs */
		EnhancedInputComponent->BindAction(CameraMovementControl, ETriggerEvent::Triggered, this, &AMainCharacterController::OnCameraMoveInputTriggered);
		EnhancedInputComponent->BindAction(CameraLocationReset, ETriggerEvent::Started, this, &AMainCharacterController::OnResetCameraPosition);
		EnhancedInputComponent->BindAction(CameraLocationReset, ETriggerEvent::Completed, this, &AMainCharacterController::OnResetCameraPosition);
		EnhancedInputComponent->BindAction(MouseWheelClick, ETriggerEvent::Started, this, &AMainCharacterController::OnMouseWheelToggled);
		EnhancedInputComponent->BindAction(CameraRotateByMouse, ETriggerEvent::Triggered, this, &AMainCharacterController::OnCameraRotateInputByMouse);
		EnhancedInputComponent->BindAction(CameraRotateByKeyboard, ETriggerEvent::Triggered, this, &AMainCharacterController::OnCameraRotateInputByKeyboard);
		EnhancedInputComponent->BindAction(CameraMovementControl, ETriggerEvent::Triggered, this, &AMainCharacterController::OnCameraRotateInputByMouse);
		EnhancedInputComponent->BindAction(MouseWheelCameraZoom, ETriggerEvent::Triggered, this, &AMainCharacterController::OnCameraZoomInputTriggered);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}
}

/* ===== Character Movement ==== */
/* New Destination set, stopping movement */
void AMainCharacterController::OnInputStarted()
{
	StopMovement();
}

/* Mouse Button is on pressed, Follow mouse cursor */
void AMainCharacterController::OnSetDestinationTriggered()
{
	HoldTime += GetWorld()->GetDeltaSeconds();
	
	FHitResult Hit;
	if(GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		TargetDestination = Hit.Location;
	}
	
	if(HoldTime < FollowDelay)
	{
		return;
	}
	
	MoveToTargetDestination();
}

void AMainCharacterController::OnSetDestinationCompleted()
{
	MoveToTargetDestination();
	HoldTime = 0.f;
}

void AMainCharacterController::MoveToTargetDestination()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetDestination);
	}
}

/* ===== Camera Movement ==== */
void AMainCharacterController::OnCameraMoveInputTriggered(const FInputActionValue& Value)
{
	FVector2D InputMovementValue = Value.Get<FVector2D>();
	
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn)
	{
		ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
		if(ControlledCharacter)
		{
			CameraActor->OnCameraMoveInputTriggered(InputMovementValue);
		}
	}
}

void AMainCharacterController::OnCameraRotateInputByMouse(const FInputActionValue& Value)
{
	if(!bIsMouseWheelClicked)
	{
		return;
	}

	float InputRotationValue = Value.Get<float>();
	CameraActor->OnCameraRotateInputTriggered(InputRotationValue);
}

void AMainCharacterController::OnCameraRotateInputByKeyboard(const FInputActionValue& Value)
{
	if(bIsMouseWheelClicked == true)
	{
		return;
	}

	float InputRotationValue = Value.Get<float>();
	CameraActor->OnCameraRotateInputTriggered(InputRotationValue);
}

void AMainCharacterController::OnCameraZoomInputTriggered(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	CameraActor->OnCameraZoomInputTriggered(ZoomValue);
}

void AMainCharacterController::OnMouseWheelToggled(const FInputActionValue& Value)
{
	bIsMouseWheelClicked = Value.Get<bool>();
	CameraActor->MouseWheelState(bIsMouseWheelClicked);
}


void AMainCharacterController::OnResetCameraPosition()
{
	CameraActor->ResetCameraPosition();
}
