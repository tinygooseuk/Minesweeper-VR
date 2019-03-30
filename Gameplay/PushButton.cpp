// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "PushButton.h"

// Engine
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h" //TEMP
#include "DrawDebugHelpers.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Shared
#include "Shared/Tween/GooseTweenComponent.h"

// Game
#include "Gameplay/GameBoard.h"
#include "System/MinesweeperMotionController.h"

APushButton::APushButton()
	: bButtonFired(false)
	, bButtonFiredFromCode(false)
	, bIsMomentary(false)
	, bCanBeFlagged(false)
	, bIsFlagged(false)
	, bWaitingForControllerOut(false)
	, DownTimer(0.0f)
	, HoldTime(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	UStaticMesh* SM_Flag =	FIND_RESOURCE(StaticMesh,					SM_Flag,			"StaticMeshes");

	SM_Tile =				FIND_RESOURCE(StaticMesh,					SM_Tile,			"StaticMeshes");
	M_Tile =				FIND_RESOURCE(MaterialInterface,			M_Controller,		"Materials");
	M_DebugHighlight =		FIND_RESOURCE(MaterialInterface,			M_DebugHighlight,	"Materials");
	
	SND_ButtonClick =		FIND_RESOURCE(SoundWave,					ButtonClick,		"Sounds");
	Haptic_ButtonPress =	FIND_RESOURCE(HapticFeedbackEffect_Curve,	Haptic_ButtonPress, "Haptics");
	
	//

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(RootComp);

	// Main mesh
	TileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMeshComponent->SetStaticMesh(SM_Tile);
	TileMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	TileMeshComponent->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f));
	TileMeshComponent->SetupAttachment(RootComp);

	// Hand detector
	static FName NAME_OverlapVRController = TEXT("OverlapVRController");
	HandDetectionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HandDetectionBox"));
	HandDetectionBoxComponent->InitBoxExtent(FVector(5.0f, 40.0f, 40.0f));
	HandDetectionBoxComponent->SetRelativeLocation(FVector(15.0f, 0.0f, 0.0f));
	HandDetectionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APushButton::OnBeginOverlap);
	HandDetectionBoxComponent->OnComponentEndOverlap.AddDynamic(this, &APushButton::OnEndOverlap);
	HandDetectionBoxComponent->SetCollisionProfileName(NAME_OverlapVRController);
	HandDetectionBoxComponent->SetGenerateOverlapEvents(true);
	HandDetectionBoxComponent->SetupAttachment(RootComp);

	// Flag mesh
	FlagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	FlagMeshComponent->SetRelativeLocationAndRotation(FVector(10.0f, 0.0f, 0.0f), FRotator(-90.0f, 0.0f, 0.0f));
	FlagMeshComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	FlagMeshComponent->bAbsoluteScale = true;
	FlagMeshComponent->SetStaticMesh(SM_Flag);
	FlagMeshComponent->SetVisibility(false);
	FlagMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	FlagMeshComponent->SetupAttachment(GetRootComponent());
}

void APushButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	if (bCanBeFlagged && !WasButtonPressed() && TrackedController.IsValid() && TrackedController->IsHoldingFlag() && TrackedController->IsTriggerHeld())
	{
		// Get flagged
		TrackedController->SetHoldingFlag(false);
		SetFlagVisible(TrackedController->GetFlagTransform());

		return;
	}

	// Get pushed
	bool bTriggerHeld = (TrackedController.IsValid() && TrackedController->IsTriggerHeld() && !TrackedController->IsHoldingFlag() && !bWaitingForControllerOut) || bButtonFiredFromCode;
	if (!bButtonFired && bTriggerHeld)
	{
		FVector TilePos = TileMeshComponent->GetRelativeTransform().GetLocation();

		if (TrackedController.IsValid())
		{
			UStaticMeshComponent* StaticMesh = TrackedController->FindComponentByClass<UStaticMeshComponent>();
			GOOSE_BAIL(StaticMesh);

			static FName NAME_Top = TEXT("Top");

			FVector HandPos = StaticMesh->GetSocketLocation(NAME_Top);
			FVector LocalHandPos = GetActorTransform().InverseTransformPositionNoScale(HandPos);

			FVector Min, Max;
			TileMeshComponent->GetLocalBounds(Min, Max);

			float TileWidth = Max.X - Min.X;
			TilePos.X = FMath::Min(TilePos.X, FMath::Clamp((LocalHandPos.X / GetActorScale3D().X) - TileWidth, 0.0f, 10.0f));
		}
		else
		{
			TilePos.X = FMath::Lerp(TilePos.X, 0.0f, 0.15f);
		}

		TileMeshComponent->SetRelativeLocation(TilePos);

		if (FMath::IsNearlyZero(TilePos.X, 0.1f))
		{
			if (IsFlagVisible())
			{
				SetFlagInvisible();
			}
			else
			{
				bButtonFired = true;
				OnButtonPressed();
			}
		}
	}
	else if (!bButtonFired)
	{
		FVector TilePos = TileMeshComponent->GetRelativeTransform().GetLocation();
		TilePos.X = FMath::Lerp(TilePos.X, 10.0f, 0.15f);
		TileMeshComponent->SetRelativeLocation(TilePos);
	}

	// Get unpushed
	if (bIsMomentary && WasButtonPressed())
	{
		DownTimer += DeltaTime;

		if (DownTimer > HoldTime)
		{
			UnPressButton();
			DownTimer = 0.0f;
		}
	}
}

void APushButton::PressButton()
{
	bButtonFiredFromCode = true;
}

void APushButton::UnPressButton()
{
	bButtonFired = false;
	bButtonFiredFromCode = false;

	SetHighlighted(false);
}

bool APushButton::WasButtonPressed() const
{
	return bButtonFiredFromCode || bButtonFired;
}

void APushButton::SetHighlighted(bool bInDebugHighlighted)
{
	TileMeshComponent->SetMaterial(0, bInDebugHighlighted ? M_DebugHighlight : M_Tile);
}


void APushButton::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMinesweeperMotionController* Controller = Cast<AMinesweeperMotionController>(OtherActor);
	if (Controller)
	{
		TrackedController = Controller;
	}
}

void APushButton::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMinesweeperMotionController* Controller = Cast<AMinesweeperMotionController>(OtherActor);
	if (Controller)
	{
		TrackedController = nullptr;
		bWaitingForControllerOut = false;
	}
}

void APushButton::SetFlagVisible(FTransform SourceTransform)
{
	// Sort of inequality check...
	if (!SourceTransform.Equals(FTransform::Identity))
	{
		RunGooseTween
		(
			GT_Ease(ElasticOut, GT_Lambda(0.8f, 0.0f, 1.0f, [=](float Alpha) 
			{		
				FTransform DestinationTransform = FlagMeshComponent->GetComponentTransform();
			
				FVector P1 = SourceTransform.GetTranslation();
				FVector P2 = DestinationTransform.GetTranslation();

				FQuat R1 = SourceTransform.GetRotation();
				FQuat R2 = DestinationTransform.GetRotation();

				FVector S1 = SourceTransform.GetScale3D();
				FVector S2 = DestinationTransform.GetScale3D();

				FTransform FinalTransform;
				FinalTransform.SetTranslation(FMath::Lerp(P1, P2, Alpha));
				FinalTransform.SetRotation(FMath::Lerp(R1, R2, Alpha));
				FinalTransform.SetScale3D(FMath::Lerp(S1, S2, Alpha));
				
				FlagMeshComponent->SetWorldTransform(FinalTransform);
			}))
		);
	}
	
	Internal_SetFlagVisible(true);	
}

void APushButton::SetFlagInvisible()
{
	Internal_SetFlagVisible(false);
}

bool APushButton::IsFlagVisible() const
{
	return bIsFlagged;
}

void APushButton::OnButtonPressed()
{
	// Call BP event, if any
	OnButtonPressed_BP.Broadcast();

	// Set highlighted
	SetHighlighted(true);

	// Play sound
	UGameplayStatics::PlaySoundAtLocation(this, SND_ButtonClick, GetActorLocation(), 1.0f, FMath::RandRange(0.8f, 1.2f));

	// Play haptics too, but only if manually pressed
	if (TrackedController.IsValid())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		GOOSE_BAIL(PC);

		PC->PlayHapticEffect(Haptic_ButtonPress, TrackedController->GetHand());
	}
}

void APushButton::Internal_SetFlagVisible(bool bInVisible)
{
	bIsFlagged = bInVisible;
	FlagMeshComponent->SetVisibility(bInVisible);

	bWaitingForControllerOut = true;
}