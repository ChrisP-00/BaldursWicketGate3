// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterCameraActor.generated.h"

class UFloatingPawnMovement;
class USplineComponent;
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
	void LockCameraToCharacter();
	UFUNCTION()
	void MouseWheelState(const bool bIsPressed);
	UFUNCTION()
	void OnCameraZoomInputTriggered(const float ZoomValue);
	UFUNCTION()
	void OnCameraRotateInputTriggered(const float InputValue);
	
		
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* TargetActor;

	/* Components */
	UPROPERTY()
	USceneComponent* RootComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;
	
	/* Is Mouse Button Pressed */
	bool bIsCameraPositionSet = false;	// Is Follow Mode
	bool bIsMouseWheelClicked = false;	// Is Camera rotated by mouse

	/* Lock to Character */
	bool bIsLockToCharacter = true;
	
	
	/* Camera Control */
	FVector2D CameraInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float CameraMovingSpeed = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float MaxCameraDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraArmLength = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator CameraArmAngle = FRotator(-60.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraLocationDeltaTime = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraRotationDeltaTime = 5.f;
	
	// Camera Pictch and Zoom Distance Control Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float ZoomSpeed = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float CurrentZoomValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float TargetZoomValue = 0;

	// Spline Setting Values 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	FVector CameraSplinePosition = FVector(-170.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	FVector SplinePointTopView = FVector(-230.f, 0.f,370.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	FVector SplinePointShoulderView = FVector(50.f, 0.f,45.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	FVector TopPointCustomTangent = FVector(119.f, 0.f,-780.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	FVector ShoulderPointCustomTangent = FVector(480.f, 0.f,-80.f);
	float SplineLength = 0.f;
	
	// Camera Rotation Control Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess=true))
	float CameraRotationSpeed = 1.f;

	
};
