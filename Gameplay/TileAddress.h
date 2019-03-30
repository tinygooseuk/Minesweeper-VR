// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"

#define GRID_SIZE	(4)

/////////////////////////
// GameBoardFace Enum 
/////////////////////////
// Defines which face we are talking about
enum class EGameBoardFace : int8
{
	GBF_Front,
	GBF_Left,
	GBF_Back,
	GBF_Right,
	GBF_Top,
	GBF_Bottom,

	GBF_MAX,
	GBF_INVALID = -1,
	GBF_FIRST = 0
};
/////

/////////////////////////
// Tile Address
/////////////////////////
// Defines where on the board the tile is found
struct FTileAddress
{
	FTileAddress() 
		: Face(EGameBoardFace::GBF_INVALID)
	{
	}
	FTileAddress(EGameBoardFace Face, int8 X, int8 Y)
		: Face(Face)
		, X(X), Y(Y)
	{

	}

	EGameBoardFace Face;
	int8 X;
	int8 Y;

	// Valid iff on a valid face and within grid size
	bool IsValid() const
	{
		return Face != EGameBoardFace::GBF_INVALID && X >= 0 && X < GRID_SIZE && Y >= 0 && Y < GRID_SIZE;
	}
	
	bool operator==(const FTileAddress& Other) const
	{
		return Face == Other.Face && X == Other.X && Y == Other.Y;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Face %d: %d, %d"), int8(Face)+1, X+1, Y+1);
	}
};
FORCEINLINE uint32 GetTypeHash(const FTileAddress& TileAddress)
{
	// Hash = (4 bytes) [0000][Face][XXXX][YYYY]
	return (uint32)(((int8)TileAddress.Face << 16) | (TileAddress.X << 8) | (TileAddress.Y << 0));
}
/////

/////////////////////////
// FaceDirection Enum 
/////////////////////////
// Defines which direction another face is in
enum class EGameBoardFaceDirection : int8
{
	GBFD_North,
	GBFD_East,
	GBFD_South,
	GBFD_West,

	GBFD_MAX,
	GBFD_INVALID = -1,
	GBFD_FIRST = 0
};
/////

/////////////////////////
// FFaceMapping
/////////////////////////
// Maps a face to each adjacent face
struct FFaceMapping
{
	FFaceMapping(EGameBoardFace InToFace, EGameBoardFaceDirection InReadFrom, bool bInInverted)
		: ToFace(InToFace)
		, ReadFrom(InReadFrom)
		, bInverted(bInInverted)
	{
	}

	// The face we are going TO
	EGameBoardFace ToFace;
	// The area to read from
	EGameBoardFaceDirection ReadFrom;
	// Whether we invert the axis
	bool bInverted;

	FTileAddress MapAddress(int8 Axis)
	{
		// Invert if needed
		if (bInverted)
		{
			Axis = GRID_SIZE - Axis - 1;
		}

		switch (ReadFrom)
		{
			case EGameBoardFaceDirection::GBFD_North:
				return FTileAddress(ToFace, Axis, GRID_SIZE - 1);

			case EGameBoardFaceDirection::GBFD_East:
				return FTileAddress(ToFace, GRID_SIZE - 1, Axis);

			case EGameBoardFaceDirection::GBFD_South:
				return FTileAddress(ToFace, Axis, 0);

			case EGameBoardFaceDirection::GBFD_West:
				return FTileAddress(ToFace, 0, Axis);

			default:
				UE_LOG(LogTemp, Warning, TEXT("Can't read from direction %i"), (int8)ReadFrom);
				return FTileAddress();
		}
	}
};
/////