// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "GameFramework/Pawn.h"
#include "Volumes/VRExclusionVolume.h"
#include "MinesweeperPawn.generated.h"

UCLASS()
class MINESWEEPER_VR_API AMinesweeperPawn : public APawn, public IVRExcludableActor
{
	GENERATED_BODY()

public:
	AMinesweeperPawn();

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IVRExcludableActor:
	void OnInsideExclusionVolumeChanged(bool bInNewInside);

	///////////////////////// Getters /////////////////////////
	// Returns true iff the pawn is not stuck with its head inside a wall
	bool IsPositionValid() const;

	///////////////////////// Inputs /////////////////////////
	UFUNCTION()
	void OnLeftGripped();
	UFUNCTION()
	void OnLeftUngripped();

	UFUNCTION()
	void OnRightGripped();
	UFUNCTION()
	void OnRightUngripped();

	UFUNCTION()
	void OnLeftTeleportPressed();
	UFUNCTION()
	void OnLeftTeleportReleased();
	
	UFUNCTION()
	void OnRightTeleportPressed();
	UFUNCTION()
	void OnRightTeleportReleased();
		
	UFUNCTION()
	void OnLeftTriggerPressed();
	UFUNCTION()
	void OnLeftTriggerReleased();
	
	UFUNCTION()
	void OnRightTriggerPressed();
	UFUNCTION()
	void OnRightTriggerReleased();

private:
	///////////////////////// Functions /////////////////////////
	// Set various inputs as pressed on either hand
	void SetControllerGripped(bool bInGripValue, bool bIsLeft);
	void SetControllerTeleportPressed(bool bInTeleportValue, bool bIsLeft);
	void SetControllerTriggerPressed(bool bInTriggerValue, bool bIsLeft);

	///////////////////////// State /////////////////////////
	// True if the position of this pawn is not within a wall/object
	bool bPositionIsValid;

	///////////////////////// Components /////////////////////////
	// Main game camera
	UPROPERTY()
	class UCameraComponent* CameraComponent;

	// Left motion controller
	UPROPERTY()
	class UMotionControllerComponent* LeftMotionControllerComponent;

	// Left controller actor
	UPROPERTY()
	class UChildActorComponent* LeftControllerActorComponent;

	// Right motion controller
	UPROPERTY()
	class UMotionControllerComponent* RightMotionControllerComponent;
	
	// Right controller actor
	UPROPERTY()
	class UChildActorComponent* RightControllerActorComponent;
};
