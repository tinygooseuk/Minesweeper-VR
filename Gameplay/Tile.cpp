// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "Tile.h"

// Engine
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h" //TEMP
#include "DrawDebugHelpers.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Game
#include "Gameplay/GameBoard.h"
#include "System/MinesweeperMotionController.h"

ATile::ATile()
	: ParentBoard(nullptr)
	, bComputedBombCount(false)
{
	// Resources
	UMaterialInterface* M_EmissiveText = FIND_RESOURCE(MaterialInterface, M_EmissiveText, "Materials");

	//
	
	DebugTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DebugText"));
	DebugTextComponent->SetMaterial(0, M_EmissiveText);
	DebugTextComponent->SetRelativeLocation(FVector(11.0f, 0.0f, 0.0f));
	DebugTextComponent->SetText(FText::FromString(TEXT("-")));
	DebugTextComponent->SetWorldSize(26.0f);
	DebugTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	DebugTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	DebugTextComponent->SetupAttachment(TileMeshComponent);
	DebugTextComponent->SetHiddenInGame(true);

	bCanBeFlagged = true;
}

void ATile::Init(FTileAddress InAddress, AGameBoard* InParentBoard)
{
	GOOSE_CHECK(InAddress.IsValid());
	Address = InAddress;
	ParentBoard = InParentBoard;

#if WITH_EDITOR
	SetActorLabel(FString::Printf(TEXT("%s Face: (%i, %i)"), *AGameBoard::GetNameForFace(InAddress.Face), InAddress.X, InAddress.Y));
#endif
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Fill info from board
	if (!bComputedBombCount && ParentBoard)
	{
		if (ParentBoard->IsBomb(Address))
		{
			DebugTextComponent->SetText(FText::FromString(TEXT("B")));
		}
		else
		{
			int8 BombsAround = ParentBoard->GetNumberOfBombsSurrounding(Address);
			DebugTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%i"), BombsAround)));
		}

		bComputedBombCount = true;
	}
}


void ATile::OnButtonPressed()
{
	Super::OnButtonPressed();

	int8 BombsAround = ParentBoard->GetNumberOfBombsSurrounding(Address);
	bool bIsBomb = ParentBoard->IsBomb(Address);

	if (bIsBomb)
	{	
		// Shatter game board
		ParentBoard->Shatter();
	}
	else if (BombsAround == 0) // No bombs - activate next ring around
	{
		ParentBoard->NotifyTileUncovered(this);

		for (FTileAddress Address : AGameBoard::GetAdjacentTiles(Address))
		{
			if (!Address.IsValid())
			{
				continue;
			}

			ATile* OtherTile = ParentBoard->GetTile(Address);
			GOOSE_BAIL_CONTINUE(OtherTile);

			OtherTile->PressButton();
		}
	}
	else 
	{
		ParentBoard->NotifyTileUncovered(this);
		DebugTextComponent->SetHiddenInGame(false);
	}	
}