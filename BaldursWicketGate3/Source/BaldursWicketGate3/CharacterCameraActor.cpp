// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCameraActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	/* Character Movement Component */
	CharacterMovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Character Movement"));
	CharacterMovementComp->bOrientRotationToMovement = true;
	CharacterMovementComp->bConstrainToPlane = true;

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
	
	/* ==== Debugging ==== */
	SpringArm->bDrawDebugLagMarkers = true;
	
	UStaticMeshComponent* Dummy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	Dummy->SetupAttachment(RootComp);

	// 큐브 메시를 기본 큐브로 설정 (UE 에디터 내 기본 제공 메시 사용)	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DummyAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (DummyAsset.Succeeded())
	{
		Dummy->SetStaticMesh(DummyAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Asset!"));
	}

	// 큐브 크기와 위치 설정
	Dummy->SetRelativeScale3D(FVector(1.f)); // 큐브 크기 조정
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

void ACharacterCameraActor::OnCameraZoomInputTriggered(const float ZoomValue)
{
	float NewPitch = ZoomValue * ZoomSpeed;
	
	CurrentZoomValue += NewPitch;
	CurrentZoomValue = FMath::Clamp(CurrentZoomValue, 0.f, 1.f);

	SpringArm->SetWorldLocation(SplineComponent->GetLocationAtTime(CurrentZoomValue, ESplineCoordinateSpace::World));

	FRotator CurrentRotation = SpringArm->GetComponentRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraComp->GetComponentLocation(), this->GetActorLocation());
	FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), CameraRotationDeltaTime);
	
	SpringArm->SetWorldRotation(SmoothRotation);	
}

void ACharacterCameraActor::OnCameraRotateInputTriggered(const float InputValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += InputValue * CameraRotationSpeed;
	SetActorRotation(NewRotation, ETeleportType::None);
}
