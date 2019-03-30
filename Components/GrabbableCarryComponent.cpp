// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "GrabbableCarryComponent.h"

// Engine
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

// Game
#include "System/MinesweeperMotionController.h"

UGrabbableCarryComponent::UGrabbableCarryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}