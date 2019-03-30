// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "MinesweeperGameModeBase.h"

// Game
#include "System/MinesweeperPawn.h"

AMinesweeperGameModeBase::AMinesweeperGameModeBase()
{
	DefaultPawnClass = AMinesweeperPawn::StaticClass();
}