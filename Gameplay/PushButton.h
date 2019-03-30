// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "GameFramework/Actor.h"
#include "Gameplay/TileAddress.h"
#include "PushButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonPressedSignature);

UCLASS()
class MINESWEEPER_VR_API APushButton : public AActor
{
	GENERATED_BODY()
	
public:	
	APushButton();

	// AActor:
	virtual void Tick(float DeltaTime) override;

	// Set button pressed
	void PressButton();
	// Set button unpressed
	void UnPressButton();
	// True iff button was pressed
	bool WasButtonPressed() const;

	// Set if button is highlighted
	void SetHighlighted(bool bInHighlighted);

	// Overlap events
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Flags
	// Set flag visible/invisible with optional transform for animation
	void SetFlagVisible(FTransform SourceTransform = FTransform::Identity);
	void SetFlagInvisible();
	// True iff flag visible
	bool IsFlagVisible() const;

	///////////////////////// Events /////////////////////////
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, DisplayName = "On Button Pressed", Category="VR Interaction")
	FButtonPressedSignature OnButtonPressed_BP;

	///////////////////////// Properties /////////////////////////
	// Button pops back up after press
	UPROPERTY(EditAnywhere)
	bool bIsMomentary;

	// If button can be pressed (mainly for tiles. Holy OOP Batman!)
	UPROPERTY(EditAnywhere)
	bool bCanBeFlagged;

	// If momentary, time that button remains depressed
	UPROPERTY(EditAnywhere)
	float HoldTime;

protected:
	///////////////////////// Components /////////////////////////
	// Tile mesh
	UPROPERTY()
	class UStaticMeshComponent* TileMeshComponent;

	// Box component for detecting VR hands
	UPROPERTY()
	class UBoxComponent* HandDetectionBoxComponent;

	// The flag mesh component
	UPROPERTY()
	class UStaticMeshComponent* FlagMeshComponent;

	virtual void OnButtonPressed();

private:
	///////////////////////// Private Funcs /////////////////////////
	// Sets flag (in)visibility
	void Internal_SetFlagVisible(bool bInVisible);

	///////////////////////// State /////////////////////////
	// Iff button was fired by human
	bool bButtonFired;
	// Iff button was fired by script
	bool bButtonFiredFromCode;
	// Iff button is flagged
	bool bIsFlagged;
	// Iff button is waiting for controller to go away
	bool bWaitingForControllerOut;
	// Counter for how long button has been depressed
	float DownTimer;

	// The controller which is pressing this button, is any
	UPROPERTY()
	TWeakObjectPtr<class AMinesweeperMotionController> TrackedController;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UStaticMesh* SM_Tile;

	UPROPERTY()
	class UMaterialInterface* M_Tile;

	UPROPERTY()
	class UMaterialInterface* M_DebugHighlight;

	UPROPERTY()
	class USoundWave* SND_ButtonClick;
	
	UPROPERTY()
	class UHapticFeedbackEffect_Curve* Haptic_ButtonPress;
};
