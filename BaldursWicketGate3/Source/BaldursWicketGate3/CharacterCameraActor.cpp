// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCameraActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"


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
	// SpringArm->SetRelativeRotation(CameraArmAngle);

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = false;

	/* Create Camera */
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = false;
	
	// spline point , point type, spline position
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(RootComp);
	SplineComponent->SetRelativeLocation(CameraSplinePosition);
	int32 TopPoint = SplineComponent->GetNumberOfSplinePoints() -2;
	int32 ShoulderPoint = SplineComponent->GetNumberOfSplinePoints() -1;
	SplineComponent->SetLocationAtSplinePoint(TopPoint, SplinePointTopView, ESplineCoordinateSpace::Local);
	SplineComponent->SetLocationAtSplinePoint(ShoulderPoint, SplinePointShoulderView, ESplineCoordinateSpace::Local);
	SplineComponent->SetTangentAtSplinePoint(TopPoint, TopPointCustomTangent, ESplineCoordinateSpace::Local);
	SplineComponent->SetTangentAtSplinePoint(ShoulderPoint, ShoulderPointCustomTangent, ESplineCoordinateSpace::Local);
	SplineComponent->UpdateSpline();
	SplineLength = SplineComponent->GetSplineLength();
	
	/* ==== Debugging ==== */
	SpringArm->bDrawDebugLagMarkers = true;
	
	UStaticMeshComponent* Dummy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	Dummy->SetupAttachment(RootComp);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DummyAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (DummyAsset.Succeeded())
	{
		Dummy->SetStaticMesh(DummyAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Asset!"));
	}
	
	Dummy->SetRelativeScale3D(FVector(1.f)); 
	Dummy->SetRelativeLocation(FVector::ZeroVector);
	Dummy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	FRotator SpringArmRotation = UKismetMathLibrary::FindLookAtRotation(CameraComp->GetComponentLocation(), this->GetActorLocation());
	SpringArm->SetWorldRotation(SpringArmRotation);

	RepositionCamera();
}

void ACharacterCameraActor::OnCameraMoveInputTriggered(const FVector2D& InputMovementValue)
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();

	FVector Movement = (ForwardVector * InputMovementValue.Y) + (RightVector * InputMovementValue.X);
	Movement *= CameraMovingSpeed;

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation + Movement;
	FVector TargetActorLocation = TargetActor->GetActorLocation();

	float CurrentDistanceToTarget = FVector::Dist(CurrentLocation, TargetActorLocation);
	float NewDistanceToTarget = FVector::Dist(NewLocation, TargetActorLocation);

	if(NewDistanceToTarget >= MaxCameraDistance)
	{
		FVector DirectionToTarget = (TargetActorLocation - CurrentLocation).GetSafeNormal();
		FVector DesiredMovementDirection = (NewLocation - CurrentLocation).GetSafeNormal();

		float DotProduct = FVector::DotProduct(DirectionToTarget, DesiredMovementDirection);

		if(DotProduct <= 0 || NewDistanceToTarget >= CurrentDistanceToTarget)
		{
			return;
		}
	}
		
	NewLocation.Z = TargetActorLocation.Z;
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

void ACharacterCameraActor::OnCameraZoomInputTriggered(const float ZoomValue)
{
	float NewPitch = ZoomValue * ZoomSpeed;
	TargetZoomValue = FMath::Clamp(CurrentZoomValue + NewPitch, 0.f, 1.f);
	
	float TargetDistance = TargetZoomValue * SplineLength;
	
	FVector CurrentLocation = SpringArm->GetComponentLocation();
	FVector TargetLocation = SplineComponent->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
	FVector SmoothLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), CameraLocationDeltaTime);
	SpringArm->SetWorldLocation(SmoothLocation);

	FRotator CurrentRotation = SpringArm->GetComponentRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraComp->GetComponentLocation(), this->GetActorLocation());
	FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), CameraRotationDeltaTime);	
	SpringArm->SetWorldRotation(SmoothRotation);

	CurrentZoomValue = TargetZoomValue;
}

void ACharacterCameraActor::OnCameraRotateInputTriggered(const float InputValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += InputValue * CameraRotationSpeed;
	SetActorRotation(NewRotation, ETeleportType::None);
}
