// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "Components/ActorComponent.h"
#include "GrabbableCarryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINESWEEPER_VR_API UGrabbableCarryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// AActor:
	UGrabbableCarryComponent();
};
