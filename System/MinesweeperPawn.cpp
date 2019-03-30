// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "MinesweeperPawn.h"

// Engine
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"

// VR
#include "MotionControllerComponent.h"

// Game
#include "System/MinesweeperMotionController.h"

AMinesweeperPawn::AMinesweeperPawn()
	: bPositionIsValid(true)
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneRootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bLockToHmd = true;
	CameraComponent->SetupAttachment(SceneRootComponent);

	LeftMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	LeftMotionControllerComponent->SetTrackingMotionSource("Left");
	LeftMotionControllerComponent->SetupAttachment(SceneRootComponent);

	LeftControllerActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("LeftControllerActor"));
	LeftControllerActorComponent->SetChildActorClass(AMinesweeperMotionController::StaticClass());
	LeftControllerActorComponent->SetupAttachment(LeftMotionControllerComponent);


	RightMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	RightMotionControllerComponent->SetTrackingMotionSource("Right");
	RightMotionControllerComponent->SetupAttachment(SceneRootComponent);

	RightControllerActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightControllerActor"));
	RightControllerActorComponent->SetChildActorClass(AMinesweeperMotionController::StaticClass());
	RightControllerActorComponent->SetupAttachment(RightMotionControllerComponent);

}

void AMinesweeperPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMinesweeperPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMinesweeperPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const FName NAME_Grip_Left("Grip_Left"); 
	PlayerInputComponent->BindAction(NAME_Grip_Left, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnLeftGripped);
	PlayerInputComponent->BindAction(NAME_Grip_Left, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnLeftUngripped);

	const FName NAME_Grip_Right("Grip_Right");
	PlayerInputComponent->BindAction(NAME_Grip_Right, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnRightGripped);
	PlayerInputComponent->BindAction(NAME_Grip_Right, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnRightUngripped);

	const FName NAME_Teleport_Left("Teleport_Left");
	PlayerInputComponent->BindAction(NAME_Teleport_Left, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnLeftTeleportPressed);
	PlayerInputComponent->BindAction(NAME_Teleport_Left, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnLeftTeleportReleased);

	const FName NAME_Teleport_Right("Teleport_Right");
	PlayerInputComponent->BindAction(NAME_Teleport_Right, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnRightTeleportPressed);
	PlayerInputComponent->BindAction(NAME_Teleport_Right, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnRightTeleportReleased);

	const FName NAME_Trigger_Left("Trigger_Left");
	PlayerInputComponent->BindAction(NAME_Trigger_Left, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnLeftTriggerPressed);
	PlayerInputComponent->BindAction(NAME_Trigger_Left, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnLeftTriggerReleased);

	const FName NAME_Trigger_Right("Trigger_Right");
	PlayerInputComponent->BindAction(NAME_Trigger_Right, EInputEvent::IE_Pressed, this, &AMinesweeperPawn::OnRightTriggerPressed);
	PlayerInputComponent->BindAction(NAME_Trigger_Right, EInputEvent::IE_Released, this, &AMinesweeperPawn::OnRightTriggerReleased);
}

void AMinesweeperPawn::OnInsideExclusionVolumeChanged(bool bInNewInside)
{
	bPositionIsValid = !bInNewInside;

	APlayerController* PC = Cast<APlayerController>(GetController());
	GOOSE_BAIL(PC);

	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	GOOSE_BAIL(CameraManager);

	if (bPositionIsValid)
	{
		CameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor::Black, true, true);
	}
	else
	{
		CameraManager->StartCameraFade(0.0f, 1.0f, 0.2f, FLinearColor::Black, true, true);
	}
}

bool AMinesweeperPawn::IsPositionValid() const
{
	return bPositionIsValid;
}

void AMinesweeperPawn::OnLeftGripped()
{
	SetControllerGripped(true, true);
}
void AMinesweeperPawn::OnLeftUngripped()
{
	SetControllerGripped(false, true);
}

void AMinesweeperPawn::OnRightGripped()
{
	SetControllerGripped(true, false);
}

void AMinesweeperPawn::OnRightUngripped()
{
	SetControllerGripped(false, false);
}

void AMinesweeperPawn::OnLeftTeleportPressed()
{
	SetControllerTeleportPressed(true, true);
}
void AMinesweeperPawn::OnLeftTeleportReleased()
{
	SetControllerTeleportPressed(false, true);
}

void AMinesweeperPawn::OnRightTeleportPressed()
{
	SetControllerTeleportPressed(true, false);
}
void AMinesweeperPawn::OnRightTeleportReleased()
{
	SetControllerTeleportPressed(false, false);
}

void AMinesweeperPawn::OnLeftTriggerPressed()
{
	SetControllerTriggerPressed(true, true);
}
void AMinesweeperPawn::OnLeftTriggerReleased()
{
	SetControllerTriggerPressed(false, true);
}

void AMinesweeperPawn::OnRightTriggerPressed()
{
	SetControllerTriggerPressed(true, false);
}
void AMinesweeperPawn::OnRightTriggerReleased()
{
	SetControllerTriggerPressed(false, false);
}

void AMinesweeperPawn::SetControllerGripped(bool bInGripValue, bool bIsLeft)
{
	UChildActorComponent* ChildActorComponent = bIsLeft ? LeftControllerActorComponent : RightControllerActorComponent;
	GOOSE_BAIL(ChildActorComponent);

	AMinesweeperMotionController* Controller = Cast<AMinesweeperMotionController>(ChildActorComponent->GetChildActor());
	GOOSE_BAIL(Controller);

	Controller->SetGripHeld(bInGripValue);
}

void AMinesweeperPawn::SetControllerTeleportPressed(bool bInTeleportValue, bool bIsLeft)
{
	UChildActorComponent* ChildActorComponent = bIsLeft ? LeftControllerActorComponent : RightControllerActorComponent;
	GOOSE_BAIL(ChildActorComponent);

	AMinesweeperMotionController* Controller = Cast<AMinesweeperMotionController>(ChildActorComponent->GetChildActor());
	GOOSE_BAIL(Controller);

	Controller->SetTeleportHeld(bInTeleportValue);
}

void AMinesweeperPawn::SetControllerTriggerPressed(bool bInTriggerValue, bool bIsLeft)
{
	UChildActorComponent* ChildActorComponent = bIsLeft ? LeftControllerActorComponent : RightControllerActorComponent;
	GOOSE_BAIL(ChildActorComponent);

	AMinesweeperMotionController* Controller = Cast<AMinesweeperMotionController>(ChildActorComponent->GetChildActor());
	GOOSE_BAIL(Controller);

	Controller->SetTriggerHeld(bInTriggerValue);
}