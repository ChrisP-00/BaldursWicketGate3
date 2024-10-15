// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCameraActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ACharacterCameraActor::ACharacterCameraActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;
	
	/* Set Camera Arm Spring */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComp);
	SpringArm->TargetArmLength = CameraArmLength;
	SpringArm->SetRelativeRotation(CameraArmAngle);

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;

	/* Create Camera */
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = false;

	/* Character Movement Component */
	CharacterMovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Character Movement"));
	CharacterMovementComp->bOrientRotationToMovement = true;
	CharacterMovementComp->bConstrainToPlane = true;
	
	/* Debugging */
	SpringArm->bDrawDebugLagMarkers = true;
	
	UStaticMeshComponent* Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComp);

	// 큐브 메시를 기본 큐브로 설정 (UE 에디터 내 기본 제공 메시 사용)	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (CubeAsset.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cube!"));
		Cube->SetStaticMesh(CubeAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Cube!"));
	}

	// 큐브 크기와 위치 설정
	Cube->SetRelativeScale3D(FVector(1.f)); // 큐브 크기 조정
	Cube->SetRelativeLocation(FVector::ZeroVector);
}	

// Called when the game starts or when spawned
void ACharacterCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterCameraActor::RepositionCamera()
{
	SetActorLocation(TargetActor->GetActorLocation());
}


// Called every frame
void ACharacterCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterCameraActor::SetTarget(AActor* ControlledActor)
{
	TargetActor = ControlledActor;

	SpringArm->TargetArmLength = CameraArmLength;
	SpringArm->SetRelativeRotation(CameraArmAngle);

	RepositionCamera();
}

void ACharacterCameraActor::OnCameraMoveInputTriggered(const FVector2D& InputMovementValue)
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();

	FVector Movement = (ForwardVector * InputMovementValue.Y) + (RightVector * InputMovementValue.X);
	Movement *= CameraMovingSpeed;

	FVector NewLocation = GetActorLocation() + Movement;
	FVector TargetActorLocation = TargetActor->GetActorLocation();
	
	float DistanceToCharacter = FVector::Dist2D(NewLocation, TargetActorLocation);
	if(DistanceToCharacter > MaxCameraDistance)
	{
		NewLocation = GetActorLocation();
	}
		
	NewLocation.Z = GetActorLocation().Z;
	SetActorLocation(NewLocation);
}

void ACharacterCameraActor::ResetCameraPosition()
{
	SetActorLocation(TargetActor->GetActorLocation());
}

void ACharacterCameraActor::MouseWheelState(const bool bIsPressed)
{
	bIsMouseWheelClicked = bIsPressed;
}

void ACharacterCameraActor::OnCameraZoomInputTriggered(const float ZoomValue) const
{

	float NewTargetArmLength = SpringArm->TargetArmLength + (ZoomValue * ZoomSpeed);
	NewTargetArmLength = FMath::Clamp(NewTargetArmLength, MinZoomDistance, MaxZoomDistance);
		
	float NewPitch = FMath::Lerp(MinZoomPitch,MaxZoomPitch, (NewTargetArmLength - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance));

	SpringArm->TargetArmLength = NewTargetArmLength;
	SpringArm->SetRelativeRotation(FRotator(NewPitch, SpringArm->GetRelativeRotation().Yaw, 0));

}

void ACharacterCameraActor::OnCameraRotateInputTriggered(const float InputValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += InputValue * CameraRotationSpeed;
	SetActorRotation(NewRotation, ETeleportType::None);
}
