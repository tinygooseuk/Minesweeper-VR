// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "Gameplay/PushButton.h"
#include "Gameplay/TileAddress.h"
#include "Tile.generated.h"

UCLASS()
class MINESWEEPER_VR_API ATile : public APushButton
{
	GENERATED_BODY()
	
public:	
	ATile();

	// Initialises a tile with a given address
	void Init(FTileAddress InAddress, class AGameBoard* ParentBoard);

	// AActor:
	virtual void Tick(float DeltaTime) override;

	///////////////////////// Getters /////////////////////////
	FORCEINLINE FTileAddress GetAddress() const { return Address; }

	///////////////////////// Components /////////////////////////
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* DebugTextComponent;

protected:
	// Event when button is pressed (override)
	virtual void OnButtonPressed() override;

private:
	///////////////////////// State /////////////////////////
	FTileAddress Address;

	UPROPERTY()
	class AGameBoard* ParentBoard;

	// Whether or not we have computed the count of bombs around
	bool bComputedBombCount;
};
