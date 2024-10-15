// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterCameraActor.generated.h"

class UCharacterMovementComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BALDURSWICKETGATE3_API ACharacterCameraActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACharacterCameraActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTarget(AActor* ControlledActor);

	UFUNCTION()
	void OnCameraMoveInputTriggered(const FVector2D& InputMovementValue);
	void ResetCameraPosition();
	UFUNCTION()
	void MouseWheelState(const bool bIsPressed);
	UFUNCTION()
	void OnCameraZoomInputTriggered(const float ZoomValue) const;
	UFUNCTION()
	void OnCameraRotateInputTriggered(const float InputValue);
	
		
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* TargetActor;
	UPROPERTY()
	USceneComponent* RootComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovementComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraArmLength = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator CameraArmAngle = FRotator(-60.f, 0.f, 0.f);


	/* Camera Control Input Values */
	FVector2D CameraInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float CameraMovingSpeed = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MaxCameraDistance = 1000.f;

	/* Is Mouse Button Pressed */
	// Is Follow Mode
	bool bIsCameraPositionSet = false;
	// Is Camera rotated by mouse
	bool bIsMouseWheelClicked = false;
	
	// Camera Pictch and Zoom Distance Control Values
	float ZoomDistance;
	float ZoomPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MinZoomDistance = 350.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MaxZoomDistance = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MinZoomPitch = -25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MaxZoomPitch = -60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float ZoomSpeed = 50.f;
	
	// Camera Rotation Control Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float CameraRotationSpeed = 1.f;

	
	/* Functions */
	void RepositionCamera();

};
