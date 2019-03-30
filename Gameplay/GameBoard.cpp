// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "GameBoard.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/Emitter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundWave.h"

// Plugin
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"

// Game
#include "Components/GrabbableRotationComponent.h"
#include "Gameplay/Tile.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Shared/Tween/GooseTweenComponent.h"
#include "Volumes/VRExclusionVolume.h"


AGameBoard::AGameBoard()
	: bShattered(false)
	, bWon(false)
	, NumBombs(14)
	, TilesUncovered(0)
	, ConfettiParticleSystemComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	UStaticMesh* SM_Board =		FIND_RESOURCE(StaticMesh,		SM_Board,		"StaticMeshes");
	SND_Explosion =				FIND_RESOURCE(SoundWave,		Explosion,		"Sounds");
	SND_YouWin =				FIND_RESOURCE(SoundWave,		YouWin,			"Sounds");
	PFX_Explosion =				FIND_RESOURCE(ParticleSystem,	P_Explosion,	"ParticleSystems");
	PFX_Confetti =				FIND_RESOURCE(ParticleSystem,	P_Confetti,		"ParticleSystems");

	//

	// Main board mesh
	BoardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	BoardMeshComponent->SetStaticMesh(SM_Board);
	BoardMeshComponent->SetSimulatePhysics(true);
	BoardMeshComponent->SetAngularDamping(2.0f);
	BoardMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	SetRootComponent(BoardMeshComponent);

	// Grabbable
	GrabbableRotationComponent = CreateDefaultSubobject<UGrabbableRotationComponent>(TEXT("GrabbableRotation"));
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	// Lock position of board
	FBodyInstance* BI = BoardMeshComponent->GetBodyInstance();
	if (GOOSE_VERIFY(BI))
	{
		BI->bLockXTranslation = true;
		BI->bLockYTranslation = true;
		BI->bLockZTranslation = true;

		BI->SetDOFLock(EDOFMode::SixDOF);
	}	

	// Play some cheeky FX
	RunEntryEffect();
}

void AGameBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameBoard::Init(int32 InNumBombs)
{
	NumBombs = InNumBombs;

	// Create the tiles
	CreateBoard();

	// Place bombs
	PlaceBombs();
}


void AGameBoard::RunEntryEffect()
{
	// Set initial transform
	SetActorRotation(FRotator(45.0f, 45.0f, 0.0f));
	SetActorScale3D(FVector(0.01f));
	
	// Spin it!
	BoardMeshComponent->AddAngularImpulseInDegrees(FVector(0.0f, 0.0f, 100.0f), NAME_None, true);

	// Tween in the scale
	RunGooseTween
	(
		GT_Ease(ElasticOut, GT_Lambda(1.5f, 0.01f, 1.0f, [this](float Value) 
		{
			SetActorScale3D(FVector(Value));
		}))	   
	);
}

void AGameBoard::RunExitEffect()
{
	// Spin it!
	BoardMeshComponent->AddAngularImpulseInDegrees(FVector(0.0f, 0.0f, 800.0f), NAME_None, true);

	// Tween in the scale
	RunGooseTween
	(
		GT_Ease(ElasticIn, GT_Lambda(1.5f, 1.0f, 0.01f, [this](float Value)
		{
			SetActorScale3D(FVector(Value));
		}))
	);
}

void AGameBoard::CreateBoard()
{
	float Scale = 1.0f / (float)GRID_SIZE;
	float Size = Scale * 100.0f;

	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	for (int8 FaceIdx = 0; FaceIdx < (int32)EGameBoardFace::GBF_MAX; FaceIdx++)
	{
		EGameBoardFace Face = (EGameBoardFace)FaceIdx;

		FQuat FaceQuat = GetQuaternionForFace(Face);

		for (int8 X = 0; X < GRID_SIZE; X++)
		{
			for (int8 Y = 0; Y < GRID_SIZE; Y++)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				FVector Location = FVector::ZeroVector;
				Location += FVector(40.0f, 50.0f, -50.0f); // bottom left of face
				Location += FVector(0.0f, -X * Size, Y * Size); // Tile offset
				Location += FVector(0.0f, -12.5f, 12.5f); // Tile centring offset

				FTransform Transform;
				Transform.SetLocation(FaceQuat.RotateVector(Location));
				Transform.SetRotation(FaceQuat);
				Transform.SetScale3D(FVector(1.0f, Scale, Scale));
				
				ATile* Tile = World->SpawnActor<ATile>(ATile::StaticClass(), Transform, SpawnParams);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
				Tile->GetRootComponent()->AttachToComponent(BoardMeshComponent, AttachRules);
				
				FTileAddress Address = FTileAddress(Face, X, Y);
				Tile->Init(Address, this);
				Tiles.Add(Address, Tile);
			}
		}
	}
}

void AGameBoard::PlaceBombs()
{	
	for (int32 I = 0; I < NumBombs; I++)
	{
		EGameBoardFace Face = (EGameBoardFace)FMath::RandRange(0, (int8)EGameBoardFace::GBF_MAX-1);
		int8 X = FMath::RandRange(0, GRID_SIZE - 1);
		int8 Y = FMath::RandRange(0, GRID_SIZE - 1);
	
		FTileAddress Random = FTileAddress(Face, X, Y);
		if (IsBomb(Random))
		{
			// Already bomb? Try another...
			I--;
			continue;
		}

		Bombs.Add(Random, true);
	}
}

void AGameBoard::Shatter()
{
	if (bShattered)
	{
		return;
	}
	bShattered = true;

	// Play explosion sound
	UGameplayStatics::PlaySoundAtLocation(this, SND_Explosion, GetActorLocation());

	// Shatter mesh
	// Remove static mesh, add destructible
	FTransform Transform = BoardMeshComponent->GetComponentTransform();
	BoardMeshComponent->SetWorldLocation(FVector(0.0f, 0.0f, 2000.0f));
	BoardMeshComponent->DestroyComponent();

	FAttachmentTransformRules Attachment = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

	UDestructibleMesh* SM_Board_DM = UGooseUtil::GetObject<UDestructibleMesh>(TEXT("SM_Board_DM"), TEXT("StaticMeshes"));
	
	UDestructibleComponent* Destructible = NewObject<UDestructibleComponent>(this, TEXT("Destructible"));
	Destructible->SetWorldTransform(Transform);
	Destructible->SetDestructibleMesh(SM_Board_DM);
	Destructible->RegisterComponent();
	SetRootComponent(Destructible);

	Destructible->SetSimulatePhysics(true);
	Destructible->ApplyRadiusDamage(1.0f, GetActorLocation(), 10.0f, 100.0f, true);
	Destructible->AddRadialImpulse(GetActorLocation(), 10.0f, 100.0f, ERadialImpulseFalloff::RIF_Linear, true);

	// Particle FX
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	Transform.SetScale3D(FVector(2.0f));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AEmitter* Emitter = World->SpawnActor<AEmitter>(AEmitter::StaticClass(), Transform, SpawnParams);
	GOOSE_BAIL(Emitter);

	Emitter->SetTemplate(PFX_Explosion);
	Emitter->bDestroyOnSystemFinish = true;
}

bool AGameBoard::IsBomb(FTileAddress Tile) const
{
	const bool* Entry = Bombs.Find(Tile);
	if (!Entry)
	{
		return false;
	}

	return *Entry;
}

int8 AGameBoard::GetNumberOfBombsSurrounding(FTileAddress Tile) const
{
	TArray<FTileAddress> Surrounding = GetAdjacentTiles(Tile);

	int8 Count = 0;
	for (FTileAddress Address : Surrounding)
	{
		if (!Address.IsValid())
		{
			continue;
		}

		if (IsBomb(Address))
		{
			Count++;
		}
	}

	return Count;
}

FQuat AGameBoard::GetQuaternionForFace(EGameBoardFace Face)
{
	FRotator FaceEulers[6] =
	{
		FRotator(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 90.0f, 0.0f),
		FRotator(0.0f, 180.0f, 0.0f),
		FRotator(0.0f, 270.0f, 0.0f),
		FRotator(90.0f, 0.0f, 0.0f),
		FRotator(270.0f, 0.0f, 0.0f),
	};
	return FaceEulers[(int32)Face].Quaternion();
}
FString AGameBoard::GetNameForFace(EGameBoardFace Face)
{
#if WITH_EDITOR
	static FString FaceNames[6] =
	{
		TEXT("Front"),
		TEXT("Left"),
		TEXT("Back"),
		TEXT("Right"),
		TEXT("Top"),
		TEXT("Bottom"),
	};
	return FaceNames[(int32)Face];
#else
	return TEXT("Face");
#endif
}

ATile* AGameBoard::GetTile(FTileAddress Tile) const
{
	ATile* const* Entry = Tiles.Find(Tile);
	if (Entry)
	{
		return *Entry;
	}

	return nullptr;
}

void AGameBoard::NotifyTileUncovered(ATile* Tile)
{
	TilesUncovered++;

	const int32 TotalTiles = GRID_SIZE * GRID_SIZE * 6;
	if (TilesUncovered == (TotalTiles - NumBombs) && !bWon)
	{
		FVector WorldLoc = GetActorLocation();
		WorldLoc.Z = 320.0f;

		// Create confetti
		ConfettiParticleSystemComponent = NewObject<UParticleSystemComponent>(this);
		ConfettiParticleSystemComponent->SetTemplate(PFX_Confetti);
		ConfettiParticleSystemComponent->SetWorldLocation(WorldLoc);
		ConfettiParticleSystemComponent->RegisterComponent();

		// Play win sound
		UGameplayStatics::PlaySoundAtLocation(this, SND_YouWin, GetActorLocation());

		// Spin and away!
		RunExitEffect();
		
		// Remove all tiles!
		for (int8 Face = 0; Face < (int8)EGameBoardFace::GBF_MAX; Face++)
		{
			for (int8 X = 0; X < GRID_SIZE; X++)
			{
				for (int8 Y = 0; Y < GRID_SIZE; Y++)
				{
					FTileAddress Address = FTileAddress((EGameBoardFace)Face, X, Y);
					if (!GOOSE_VERIFY(Address.IsValid()))
					{
						continue;
					}

					ATile* TileToRemove = GetTile(Address);
					if (GOOSE_VERIFY(TileToRemove))
					{
						TileToRemove->Destroy();
					}
				}
			}
		}

		bWon = true;
	}
}

TArray<FTileAddress> AGameBoard::GetAdjacentTiles(FTileAddress Tile)
{
	TArray<FTileAddress> Addresses;
	Addresses.Reserve(9);

	// Work out if we're on an edge
	bool bLeftEdge = Tile.X == 0;
	bool bBottomEdge = Tile.Y == 0;
	bool bRightEdge = Tile.X == GRID_SIZE - 1;
	bool bTopEdge = Tile.Y == GRID_SIZE - 1;

	// The following code is horrendous.
	//
	// It either directly maps another FTileAddress, if the two tiles are on the same face, or
	// if they are not on the same face, asks the below function for a "Face Mapping" which
	// describes how to get from the current face to the target one. It then uses that mapping
	// to convert from face to face, taking into account the orientation and relative positions
	// of the two cube faces. 
	//
	// To be perfectly honest, I'm just very grateful that I had the concentration to finish off
	// this function, so that I never have to think about it again.

	// Left
	if (!bLeftEdge)
	{
		Addresses.Push(FTileAddress(Tile.Face, Tile.X - 1, Tile.Y));

		// Top-Left
		if (!bTopEdge)
		{
			Addresses.Push(FTileAddress(Tile.Face, Tile.X - 1, Tile.Y + 1));
		}
		else
		{
			FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_North);
			Addresses.Push(Mapping.MapAddress(Tile.X - 1));
		}

		// Bottom-Left
		if (!bBottomEdge)
		{
			Addresses.Push(FTileAddress(Tile.Face, Tile.X - 1, Tile.Y - 1));
		}
		else
		{
			FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_South);
			Addresses.Push(Mapping.MapAddress(Tile.X - 1));
		}
	}
	else
	{
		FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_West);
		
		Addresses.Push(Mapping.MapAddress(Tile.Y-1));
		Addresses.Push(Mapping.MapAddress(Tile.Y));
		Addresses.Push(Mapping.MapAddress(Tile.Y+1));
	}

	// Right
	if (!bRightEdge)
	{
		Addresses.Push(FTileAddress(Tile.Face, Tile.X + 1, Tile.Y));

		// Top-Right
		if (!bTopEdge)
		{
			Addresses.Push(FTileAddress(Tile.Face, Tile.X + 1, Tile.Y + 1));
		}
		else
		{
			FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_North);
			Addresses.Push(Mapping.MapAddress(Tile.X + 1));
		}

		// Bottom-Right
		if (!bBottomEdge)
		{
			Addresses.Push(FTileAddress(Tile.Face, Tile.X + 1, Tile.Y - 1));
		}
		else
		{
			FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_South);
			Addresses.Push(Mapping.MapAddress(Tile.X + 1));
		}
	}
	else
	{
		FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_East);

		Addresses.Push(Mapping.MapAddress(Tile.Y - 1));
		Addresses.Push(Mapping.MapAddress(Tile.Y));
		Addresses.Push(Mapping.MapAddress(Tile.Y + 1));
	}

	// Top
	if (Tile.Y < GRID_SIZE - 1)
	{
		Addresses.Push(FTileAddress(Tile.Face, Tile.X, Tile.Y + 1));
	}
	else
	{
		FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_North);
		Addresses.Push(Mapping.MapAddress(Tile.X));
	}

	// Bottom
	if (Tile.Y > 0)
	{
		Addresses.Push(FTileAddress(Tile.Face, Tile.X, Tile.Y - 1));
	}
	else
	{
		FFaceMapping Mapping = GetFaceMapping(Tile.Face, EGameBoardFaceDirection::GBFD_South);
		Addresses.Push(Mapping.MapAddress(Tile.X));
	}
	
	// Well done for staying with us till now; you may want to grab a coffee after that!

	return Addresses;
}

FFaceMapping AGameBoard::GetFaceMapping(EGameBoardFace From, EGameBoardFaceDirection Direction)
{
	// This function is a beast.
	// Basically this takes a game board face and a NESW direction from that face.
	// It returns the face in that direction, along with some data about how the two faces
	// connect. 
	// For example, the first entry:
	//
	//	// Front
	//  {
	//     /*N*/FFaceMapping(EGameBoardFace::GBF_Top,		EGameBoardFaceDirection::GBFD_South,	false),
	//
	// denotes that, North of the Front face appears the Top face, and the edge it shares with the Front face
	// is its South/bottom edge, which is not inverted in comparison to the North/top of the Front face.
	// 
	// Using this mapping, you can then find out which grid tiles are adjacent across faces.

	static FFaceMapping Mappings[6][4] =
	{
		// Front
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Top,		EGameBoardFaceDirection::GBFD_South,	false),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Right,	EGameBoardFaceDirection::GBFD_West,		false),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Bottom,	EGameBoardFaceDirection::GBFD_North,	false),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Left,		EGameBoardFaceDirection::GBFD_East,		false),
		},

		// Left
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Top,		EGameBoardFaceDirection::GBFD_West,		true),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Front,	EGameBoardFaceDirection::GBFD_West,		false),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Bottom,	EGameBoardFaceDirection::GBFD_West,		false),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Back,		EGameBoardFaceDirection::GBFD_East,		false),
		},

		// Back
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Top,		EGameBoardFaceDirection::GBFD_North,	true),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Left,		EGameBoardFaceDirection::GBFD_West,		false),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Bottom,	EGameBoardFaceDirection::GBFD_South,	true),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Right,	EGameBoardFaceDirection::GBFD_East,		false),
		},

		// Right
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Top,		EGameBoardFaceDirection::GBFD_East,		false),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Back,		EGameBoardFaceDirection::GBFD_West,		false),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Bottom,	EGameBoardFaceDirection::GBFD_East,		true),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Front,	EGameBoardFaceDirection::GBFD_East,		false),
		},

		// Top 
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Back,		EGameBoardFaceDirection::GBFD_North,	true),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Right,	EGameBoardFaceDirection::GBFD_North,	false),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Front,	EGameBoardFaceDirection::GBFD_North,	false),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Left,		EGameBoardFaceDirection::GBFD_North,	true),
		},

		// Bottom
		{
		/*N*/FFaceMapping(EGameBoardFace::GBF_Front,	EGameBoardFaceDirection::GBFD_South,	false),
		/*E*/FFaceMapping(EGameBoardFace::GBF_Right,	EGameBoardFaceDirection::GBFD_South,	true),
		/*S*/FFaceMapping(EGameBoardFace::GBF_Back,		EGameBoardFaceDirection::GBFD_North,	true),
		/*W*/FFaceMapping(EGameBoardFace::GBF_Left,		EGameBoardFaceDirection::GBFD_South,	false),
		},
	};

	return Mappings[(int32)From][(int32)Direction];
}
