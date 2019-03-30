// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "GameFramework/Volume.h"
#include "VRExclusionVolume.generated.h"

UINTERFACE()
class MINESWEEPER_VR_API UVRExcludableActor : public UInterface
{
	GENERATED_BODY()
};

class MINESWEEPER_VR_API IVRExcludableActor 
{
	GENERATED_BODY()

public:
	virtual void OnInsideExclusionVolumeChanged(bool bInNewInside) = 0;
};


UCLASS()
class MINESWEEPER_VR_API AVRExclusionVolume : public AVolume
{
	GENERATED_BODY()

public:
	AVRExclusionVolume();
	virtual void Tick(float DeltaSeconds) override;
		
private:
	bool bHeadsetWasInside;
};
