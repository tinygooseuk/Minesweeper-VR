// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "VRExclusionVolume.h"

// Engine
#include "Kismet/GameplayStatics.h"

// VR
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

AVRExclusionVolume::AVRExclusionVolume()
	: bHeadsetWasInside(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVRExclusionVolume::Tick(float DeltaSeconds)
{
	FVector DeviceLocation;
	FRotator DeviceRotation;
	
	// Get device location of HMD
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DeviceLocation);
	DeviceLocation = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(this).TransformPosition(DeviceLocation);

	// Is headset inside now?
	bool bNowEncompasses = EncompassesPoint(DeviceLocation, 20.f);

	if (bNowEncompasses != bHeadsetWasInside)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		GOOSE_BAIL(PC);

		IVRExcludableActor* ControlledPawn = Cast<IVRExcludableActor>(PC->GetPawn());
		GOOSE_BAIL(ControlledPawn);

		// We entered or left the exclusion volume
		ControlledPawn->OnInsideExclusionVolumeChanged(bNowEncompasses);

		bHeadsetWasInside = bNowEncompasses;
	}
}