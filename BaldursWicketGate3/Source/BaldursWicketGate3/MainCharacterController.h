// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainCharacterController.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class ACharacterCameraActor;

/**
 * 
 */
UCLASS()
class BALDURSWICKETGATE3_API AMainCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainCharacterController();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputMappingContext* DefaultInputMappingContext;


protected:
	virtual void BeginPlay() override;
	
	/* mapping context*/
	virtual void SetupInputComponent() override;

	/* Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* CharacterMovementControl;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* CameraMovementControl;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* CameraLockToCharacter;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* CameraRotateByKeyboard;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* CameraRotateByMouse;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* MouseWheelClick;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Input)
	UInputAction* MouseWheelCameraZoom;
	
	
	/* Inputs Binding Functions */
	// Character Control Inputs 
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationCompleted();
	void MoveToTargetDestination();
	
	// Camera Control Inputs
	UFUNCTION()
	void OnCameraMoveInputTriggered(const FInputActionValue& Value);		//WASD to move camera
	UFUNCTION()
	void OnCameraRotateInputByMouse(const FInputActionValue& Value);		// Mouse to rotate camera
	UFUNCTION()
	void OnCameraRotateInputByKeyboard(const FInputActionValue& Value);		// Q,E keys to rotate camera
	UFUNCTION()
	void OnCameraZoomInputTriggered(const FInputActionValue& Value);		// Mouse wheel to zoom camera
	UFUNCTION()
	void OnMouseWheelToggled(const FInputActionValue& Value);
	void OnLockCameraToCharacter();
private:

	/* Movement Input Delay Time */
	FVector TargetDestination;
	float HoldTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float FollowDelay = 0.8f;

	UPROPERTY()
	ACharacterCameraActor* CameraActor;

	bool bIsMouseWheelClicked = false;
	bool bIsFollowingMouse = false;
};	
