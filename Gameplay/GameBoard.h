// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "GameFramework/Actor.h"
#include "Gameplay/TileAddress.h"
#include "GameBoard.generated.h"

UCLASS()
class MINESWEEPER_VR_API AGameBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameBoard();

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Init(int32 NumBombs);

	// Get a tile actor, given its address. Returns nullptr for invalid addresses
	class ATile* GetTile(FTileAddress Tile) const;
	
	// Sent by tiles to acknowledge safe tile pressed
	void NotifyTileUncovered(class ATile* Tile);

	// Return all adjacent tiles for a given address. Addresses should be checked for validity.
	static TArray<FTileAddress> GetAdjacentTiles(FTileAddress Tile);

private:
	// Run a little animation on board creation
	void RunEntryEffect();
	// Run a little animation on board complete
	void RunExitEffect();
	// Creates the board - that is, the tiles that comprise the board
	void CreateBoard();
	// Place a number of bombs into the board
	void PlaceBombs();
	// Shatter the board (explosion)
	void Shatter();

	// Return true iff this tile is a bomb
	bool IsBomb(FTileAddress Tile) const;
	// Return how many bombs surround this tile
	int8 GetNumberOfBombsSurrounding(FTileAddress Tile) const;

	// Get the relative rotation of this face
	static FQuat GetQuaternionForFace(EGameBoardFace Face);
	// Get the name of this face (debug only!)
	static FString GetNameForFace(EGameBoardFace Face);
	// Get the mapping from one face to another, by specifying a direction
	static FFaceMapping GetFaceMapping(EGameBoardFace From, EGameBoardFaceDirection Direction);

	///////////////////////// State /////////////////////////
	// Tile storage
	TMap<FTileAddress, class ATile*> Tiles; 
	// Bomb storage
	TMap<FTileAddress, bool> Bombs;
	// If shattered already
	bool bShattered;
	// If won already
	bool bWon;

	// Number of bombs in the board
	int NumBombs;
	// Number of tiles we've uncovered successfully
	int TilesUncovered;

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UStaticMeshComponent* BoardMeshComponent;
	
	UPROPERTY()
	class UGrabbableRotationComponent* GrabbableRotationComponent;

	UPROPERTY()
	class UParticleSystemComponent* ConfettiParticleSystemComponent;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class USoundWave* SND_Explosion;

	UPROPERTY()
	class USoundWave* SND_YouWin;

	UPROPERTY()
	class UParticleSystem* PFX_Explosion;

	UPROPERTY()
	class UParticleSystem* PFX_Confetti;

	friend class ATile;
};