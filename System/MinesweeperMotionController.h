// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "GameFramework/Actor.h"
#include "MinesweeperMotionController.generated.h"

UCLASS()
class MINESWEEPER_VR_API AMinesweeperMotionController : public AActor
{
	GENERATED_BODY()
	
public:	
	// AActor:
	AMinesweeperMotionController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	///////////////////////// Events /////////////////////////
	// Get/set grip held
	void SetGripHeld(bool bInGripHeld);
	bool IsGripHeld() const;

	// Get/set teleport held
	void SetTeleportHeld(bool bInTeleportHeld);
	bool IsTeleportHeld() const;

	// Get/set trigger held
	void SetTriggerHeld(bool bInTriggerHeld);
	bool IsTriggerHeld() const;

	// Get/set flag held
	void SetHoldingFlag(bool bInHoldingFlag);
	bool IsHoldingFlag() const;
	// Get the world transform of the flag
	FTransform GetFlagTransform() const;

	// Return which hand is holding this controller
	EControllerHand GetHand() const;

private:
	///////////////////////// Private Functions /////////////////////////
	// Find the Unreal motion controller that this controller is attached to
	class UMotionControllerComponent* FindParentController() const;
	// Find the Unreal Pawn which we're a part of
	class AMinesweeperPawn* FindParentPawn() const;

	// Return the list of grabbable items in reach
	TArray<class UGrabbableRotationComponent*> GetInReachGrabbableRotationComponents() const;

	// Return the list of grabbable items in reach
	TArray<class UGrabbableCarryComponent*> GetInReachGrabbableCarryComponents() const;

	// Grab a grabbable item
	void GrabItem(class UGrabbableCarryComponent* Grabbable);
	// Drop a grabbed item. Returns true iff something was dropped
	bool DropGrabbedItem();

	// True if the controller is reaching behind the player's back
	bool IsControllerInBackpack() const;

	// Update the state of the teleport mechanism
	void UpdateTeleport();

	///////////////////////// State /////////////////////////
	// If grip button is held
	bool bGripHeld;
	
	// If teleport button is held
	bool bTeleportHeld;
	// Where we would teleport if the teleport button is released
	FVector CandidateTeleportLocation;

	// If trigger button is held
	bool bTriggerHeld;
	// If we are holding a flag
	bool bFlagIsVisible;

	// Which hand is holding this controller
	EControllerHand Hand;
	// A grace timer for releasing grabbable items when out of range
	float GrabbableItemReleaseTimer;
	
	// A grabbed rotatable item (i.e. the game board)
	UPROPERTY()
	class UGrabbableRotationComponent* GrabbedRotationItem;

	// A grabbed carriable item (i.e. a prop)
	UPROPERTY()
	class UGrabbableCarryComponent* GrabbedCarryItem;

	///////////////////////// Resources /////////////////////////
	// The main controller material
	UPROPERTY()
	class UMaterialInstanceDynamic* ControllerMaterial;

	// Teleport arc graphic
	UPROPERTY()
	class UStaticMesh* SM_TeleportTube;

	// Teleport arc's material (so we can change the alpha)
	UPROPERTY()
	class UMaterialInstanceDynamic* TeleportTubeMaterial;

	// Teleport "puck" which appears on the floor
	UPROPERTY()
	class UStaticMesh* SM_Puck;

	// Haptic click when teleport location is valid
	UPROPERTY()
	class UHapticFeedbackEffect_Curve* Haptic_NavOkay;

	///////////////////////// Components /////////////////////////
	// The main controller mesh
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ControllerMeshComponent;
	
	// The flag mesh, if one is held
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* FlagMeshComponent;

	// The teleport arc's actor
	UPROPERTY()
	class AMultiSplineMeshActor* TeleportSplineMeshActor;

	// The teleport "puck" which appears on the ground
	UPROPERTY()
	class UStaticMeshComponent* TeleportPuckComponent;
};
