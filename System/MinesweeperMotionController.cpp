// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "MinesweeperMotionController.h"

// Engine
#include "AI/NavigationSystemBase.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

// VR
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

// Lib
#include "Shared/Tween/GooseTweenComponent.h"

// Game
#include "Actors/MultiSplineMeshActor.h"
#include "Components/GrabbableCarryComponent.h"
#include "Components/GrabbableRotationComponent.h"
#include "System/MinesweeperPawn.h"

AMinesweeperMotionController::AMinesweeperMotionController()
	: bGripHeld(false)
	, bTeleportHeld(false)
	, bTriggerHeld(false)
	, bFlagIsVisible(false)
	, CandidateTeleportLocation(FVector::ZeroVector)	
	, GrabbedRotationItem(nullptr)
	, GrabbedCarryItem(nullptr)
	, GrabbableItemReleaseTimer(0.0f)
	, TeleportSplineMeshActor(nullptr)
	, TeleportPuckComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	UStaticMesh* SM_Controller =	FIND_RESOURCE(StaticMesh,					SM_ViveController,	"StaticMeshes");
	UStaticMesh* SM_Flag =			FIND_RESOURCE(StaticMesh,					SM_Flag,			"StaticMeshes");
	SM_TeleportTube =				FIND_RESOURCE(StaticMesh,					SM_TeleportTube,	"StaticMeshes");
	SM_Puck =						FIND_RESOURCE(StaticMesh,					SM_Puck,			"StaticMeshes");
	Haptic_NavOkay =				FIND_RESOURCE(HapticFeedbackEffect_Curve,	Haptic_NavOkay,		"Haptics");

	/////

	USceneComponent* RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	ControllerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMeshComponent->SetStaticMesh(SM_Controller);
	ControllerMeshComponent->SetCollisionProfileName(TEXT("VR_Controller"));
	ControllerMeshComponent->SetUseCCD(true);
	ControllerMeshComponent->SetGenerateOverlapEvents(true);
	ControllerMeshComponent->SetupAttachment(RootComponent);

	FlagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	FlagMeshComponent->SetStaticMesh(SM_Flag);
	FlagMeshComponent->SetVisibility(false);
	FlagMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	FlagMeshComponent->SetupAttachment(ControllerMeshComponent);
}

void AMinesweeperMotionController::BeginPlay()
{
	Super::BeginPlay();

	// Create material
	UMaterialInterface* OriginalMat = ControllerMeshComponent->GetMaterial(0);
	ControllerMaterial = UMaterialInstanceDynamic::Create(OriginalMat, this);
	ControllerMeshComponent->SetMaterial(0, ControllerMaterial);

	// Create teleport tube material
	UMaterialInterface* OriginalTubeMat = SM_TeleportTube->GetMaterial(0);
	TeleportTubeMaterial = UMaterialInstanceDynamic::Create(OriginalTubeMat, this);

	// Work out if left or right hand
	UMotionControllerComponent* MotionController = FindParentController();
	GOOSE_BAIL(MotionController);
	
	Hand = MotionController->GetTrackingSource();	
}

void AMinesweeperMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Check the grabbed item, if any, for release
	if (GrabbedRotationItem)
	{
		TArray<UGrabbableRotationComponent*> AvailableGrabbables = GetInReachGrabbableRotationComponents();

		if (!AvailableGrabbables.Contains(GrabbedRotationItem))
		{
			GrabbableItemReleaseTimer += DeltaTime;

			if (GrabbableItemReleaseTimer >= 0.8f)
			{
				SetGripHeld(false);
				GrabbableItemReleaseTimer = 0.0f;
			}
		}
	}

	// Drop carried item if not holding grip
	if (GrabbedCarryItem)
	{
		if (!IsGripHeld())
		{
			DropGrabbedItem();
		}
	}
	else
	{
		if (IsGripHeld())
		{
			TArray<UGrabbableCarryComponent*> AvailableGrabbables = GetInReachGrabbableCarryComponents();
			if (AvailableGrabbables.Num())
			{
				GrabItem(AvailableGrabbables[0]);
			}
		}
	}
	
	// Update teleport FX and perform if required
	UpdateTeleport();
}

void AMinesweeperMotionController::SetGripHeld(bool bInGripHeld)
{	
	GOOSE_BAIL(ControllerMaterial);

	AMinesweeperPawn* Pawn = FindParentPawn();
	GOOSE_BAIL(Pawn);

	if (bTriggerHeld || !Pawn->IsPositionValid())
	{
		// Can't trigger AND grip, and can't teleport when inside a wall
		return;
	}

	// Glow (or not!)	
	const FName NAME_Selected("Selected");
	ControllerMaterial->SetScalarParameterValue(NAME_Selected, bInGripHeld ? 1.0f : 0.0f);

	const FName NAME_Colour("Colour");
	ControllerMaterial->SetVectorParameterValue(NAME_Colour, FColor::Orange);

	// Look for grabbables and grab if possible
	TArray<UGrabbableRotationComponent*> AvailableGrabbables = GetInReachGrabbableRotationComponents();

	if (bInGripHeld)
	{
		if (AvailableGrabbables.Num() > 0) 
		{
			AvailableGrabbables[0]->AddRotator(this);
			GrabbedRotationItem = AvailableGrabbables[0];
		}
	} 
	else
	{
		if (GrabbedRotationItem)
		{
			GrabbedRotationItem->RemoveRotator(this);
			GrabbedRotationItem = nullptr;
		}
	}

	// Set collision?
	//ControllerMeshComponent->SetCollisionEnabled(bInGripHeld ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

	bGripHeld = bInGripHeld;
}

bool AMinesweeperMotionController::IsGripHeld() const
{
	return bGripHeld;
}

void AMinesweeperMotionController::SetTeleportHeld(bool bInTeleportHeld)
{
	if (!bInTeleportHeld)
	{
		if (CandidateTeleportLocation.SizeSquared() > 0.01f)
		{
			UMotionControllerComponent* MotionController = FindParentController();
			GOOSE_BAIL(MotionController);

			APawn* Player = Cast<APawn>(MotionController->GetOwner());
			GOOSE_BAIL(Player);

			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			GOOSE_BAIL(PC);

			APlayerCameraManager* CamManager = PC->PlayerCameraManager;
			GOOSE_BAIL(CamManager);
						
			// Work out time to delay based on distance
			const float Distance2D = FVector::Dist2D(Player->GetActorLocation(), CandidateTeleportLocation);
						
			constexpr float MaxDistance = 5'00.f; //5m
			constexpr float MaxTime = 0.5f;
			
			const float FadeDelayTime = FMath::Lerp(0.0f, MaxTime, FMath::Min(1.0f, (Distance2D / MaxDistance)));

			// Fade out and move player
			CamManager->SetManualCameraFade(1.0f, FColor::Black, false);
			Player->SetActorLocation(CandidateTeleportLocation);
			CandidateTeleportLocation = FVector::ZeroVector;

			// After delay, fade back in
			UGooseUtil::DoAfter(this, FadeDelayTime, [=] 
			{
				CamManager->StartCameraFade(1.0f, 0.0f, 0.3f, FColor::Black, false);
			});		
		}
	}

	bTeleportHeld = bInTeleportHeld;
}
bool AMinesweeperMotionController::IsTeleportHeld() const
{
	return bTeleportHeld;
}


void AMinesweeperMotionController::SetTriggerHeld(bool bInTriggerHeld)
{
	// Glow (or not!)
	const FName NAME_Selected("Selected");
	ControllerMaterial->SetScalarParameterValue(NAME_Selected, bInTriggerHeld ? 1.0f : 0.0f);

	const FName NAME_Colour("Colour");
	ControllerMaterial->SetVectorParameterValue(NAME_Colour, FColor::Red);

	if (bInTriggerHeld)
	{
		if (IsControllerInBackpack())
		{
			bFlagIsVisible = !bFlagIsVisible;
			SetHoldingFlag(bFlagIsVisible);
		}
	}

	bTriggerHeld = bInTriggerHeld;
}
bool AMinesweeperMotionController::IsTriggerHeld() const
{
	return bTriggerHeld;
}

void AMinesweeperMotionController::SetHoldingFlag(bool bInHoldingFlag)
{
	bFlagIsVisible = bInHoldingFlag;
	FlagMeshComponent->SetVisibility(bInHoldingFlag);
}

bool AMinesweeperMotionController::IsHoldingFlag() const
{
	return bFlagIsVisible;
}

FTransform AMinesweeperMotionController::GetFlagTransform() const
{
	return FlagMeshComponent->GetComponentTransform();
}

EControllerHand AMinesweeperMotionController::GetHand() const
{
	return Hand;
}

UMotionControllerComponent* AMinesweeperMotionController::FindParentController() const
{
	UChildActorComponent* Parent = GetParentComponent();
	GOOSE_BAIL_RETURN(Parent, nullptr);

	TArray<USceneComponent*> Parents;
	Parent->GetParentComponents(Parents);

	for (USceneComponent* Parent : Parents)
	{
		UMotionControllerComponent* MotionController = Cast<UMotionControllerComponent>(Parent);
		if (!MotionController)
		{
			continue;
		}

		return MotionController;
	}

	return nullptr;
}

AMinesweeperPawn* AMinesweeperMotionController::FindParentPawn() const
{
	UMotionControllerComponent* MotionController = FindParentController();
	GOOSE_BAIL_RETURN(MotionController, nullptr);

	return Cast<AMinesweeperPawn>(MotionController->GetOwner());
}

TArray<UGrabbableRotationComponent*> AMinesweeperMotionController::GetInReachGrabbableRotationComponents() const
{
	TArray<UGrabbableRotationComponent*> AvailableGrabbables;

	UWorld* World = GetWorld();
	GOOSE_BAIL_RETURN(World, AvailableGrabbables);

	// Get all overlappings
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByChannel
	(
		Overlaps,
		ControllerMeshComponent->GetComponentLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_WorldDynamic,
		FCollisionShape::MakeSphere(5.0f)
	);

	// Reserve enough for overlaps (won't need this many i'm sure!)
	AvailableGrabbables.Reserve(Overlaps.Num());

	// Find out if any overlaps are grabbable
	for (FOverlapResult Overlap : Overlaps)
	{
		AActor* OverlappingActor = Overlap.GetActor();
		GOOSE_BAIL_CONTINUE(OverlappingActor);

		UGrabbableRotationComponent* Grabber = OverlappingActor->FindComponentByClass<UGrabbableRotationComponent>();
		if (!Grabber)
		{
			continue;
		}

		AvailableGrabbables.Add(Grabber);
	}

	return AvailableGrabbables;
}

TArray<UGrabbableCarryComponent*> AMinesweeperMotionController::GetInReachGrabbableCarryComponents() const
{
	TArray<UGrabbableCarryComponent*> AvailableGrabbables;

	UWorld* World = GetWorld();
	GOOSE_BAIL_RETURN(World, AvailableGrabbables);

	// Get all overlappings
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByChannel
	(
		Overlaps,
		ControllerMeshComponent->GetComponentLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_WorldDynamic,
		FCollisionShape::MakeSphere(5.0f)
	);

	// Reserve enough for overlaps (won't need this many i'm sure!)
	AvailableGrabbables.Reserve(Overlaps.Num());

	// Find out if any overlaps are grabbable
	for (FOverlapResult Overlap : Overlaps)
	{
		AActor* OverlappingActor = Overlap.GetActor();
		GOOSE_BAIL_CONTINUE(OverlappingActor);

		UGrabbableCarryComponent* Grabber = OverlappingActor->FindComponentByClass<UGrabbableCarryComponent>();
		if (!Grabber)
		{
			continue;
		}

		AvailableGrabbables.Add(Grabber);
	}

	return AvailableGrabbables;
}


void AMinesweeperMotionController::GrabItem(UGrabbableCarryComponent* Grabbable)
{
	GOOSE_BAIL(Grabbable);

	// TODO: Check not already attached to something?!

	AActor* Parent = Grabbable->GetOwner();
	GOOSE_BAIL(Parent);

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Parent->GetRootComponent());
	GOOSE_BAIL(PrimComp);

	PrimComp->AttachToComponent(ControllerMeshComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	GrabbedCarryItem = Grabbable;
}
bool AMinesweeperMotionController::DropGrabbedItem()
{
	if (GrabbedCarryItem)
	{
		AActor* Parent = GrabbedCarryItem->GetOwner();
		GOOSE_BAIL_RETURN(Parent, false);

		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Parent->GetRootComponent());
		GOOSE_BAIL_RETURN(PrimComp, false);

		PrimComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PrimComp->SetSimulatePhysics(true);

		GrabbedCarryItem = nullptr;

		return true;
	}
	
	return false;
}

bool AMinesweeperMotionController::IsControllerInBackpack() const
{
	UMotionControllerComponent* MotionController = FindParentController();
	GOOSE_BAIL_RETURN(MotionController, false);

	FVector ControllerLocation = MotionController->GetComponentLocation();

	FVector DeviceLocation;
	FRotator DeviceRotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DeviceLocation);

	AMinesweeperMotionController* NonConstThis = const_cast<AMinesweeperMotionController*>(this);
	DeviceLocation = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(NonConstThis).TransformPosition(DeviceLocation);

	FVector ControllerToHMD = ControllerLocation - DeviceLocation;

	FVector HMDForward = DeviceRotation.Quaternion().GetForwardVector();
	HMDForward.Z = 0.0f;
	HMDForward.Normalize();

	float DotProduct = HMDForward | ControllerToHMD.GetSafeNormal();
	float Distance = ControllerToHMD.Size2D();

	return (DotProduct > 0.7f && Distance < 50.0f);
}

void AMinesweeperMotionController::UpdateTeleport()
{
	// Update teleport
	if (bTeleportHeld)
	{
		UWorld* World = GetWorld();
		GOOSE_BAIL(World);

		// If we don't have a teleport spline mesh then create it
		if (!TeleportSplineMeshActor)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			TeleportSplineMeshActor = World->SpawnActor<AMultiSplineMeshActor>(SpawnParams);
			TeleportSplineMeshActor->CollisionProfile = FCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			TeleportSplineMeshActor->ForwardAxis = ESplineMeshAxis::X;
			TeleportSplineMeshActor->OverrideMaterial = TeleportTubeMaterial;
			TeleportSplineMeshActor->StaticMesh = SM_TeleportTube;
		}

		if (!TeleportPuckComponent)
		{
			TeleportPuckComponent = NewObject<UStaticMeshComponent>(this);
			TeleportPuckComponent->SetStaticMesh(SM_Puck);
			TeleportPuckComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			TeleportPuckComponent->RegisterComponent();
		}

		FPredictProjectilePathParams PathParams;
		PathParams.ActorsToIgnore.Add(this);
		PathParams.ActorsToIgnore.Add(FindParentController()->GetOwner());
		PathParams.StartLocation = GetActorLocation();
		PathParams.LaunchVelocity = GetActorForwardVector() * 5'00.0f;
		PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
		PathParams.MaxSimTime = 3.0f;
		PathParams.bTraceComplex = true;
		PathParams.bTraceWithCollision = true;
		PathParams.bTraceWithChannel = true;

		FPredictProjectilePathResult PathResult;
		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

		// Update spline
		TArray<FVector> OutPoints;
		OutPoints.Reserve(PathResult.PathData.Num());

		for (const auto& PathPoint : PathResult.PathData)
		{
			OutPoints.Add(PathPoint.Location);
		}
		TeleportSplineMeshActor->SetPoints(OutPoints);

		if (PathResult.HitResult.bBlockingHit)
		{
			FVector InputVector = PathResult.HitResult.Location;
			FNavLocation OutputLocation;

			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
			GOOSE_BAIL(NavSys);

			ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::Create);
			GOOSE_BAIL(NavData);

			bool bProjected = NavSys->ProjectPointToNavigation(InputVector, OutputLocation, INVALID_NAVEXTENT, NavData, UNavigationQueryFilter::GetQueryFilter(*NavData, this, nullptr));
			bool bCandidateIsCloseEnough = (InputVector - OutputLocation.Location).Size2D() < 0.1f;

			const FName NAME_Opacity = FName("Opacity");

			if (bProjected && bCandidateIsCloseEnough)
			{
				// If only just got, play haptic effect
				if (CandidateTeleportLocation.SizeSquared() <= 0.01f)
				{
					APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
					GOOSE_BAIL(PC);

					PC->PlayHapticEffect(Haptic_NavOkay, GetHand());
				}

				CandidateTeleportLocation = OutputLocation.Location;
				TeleportTubeMaterial->SetScalarParameterValue(NAME_Opacity, 0.5f);
				TeleportPuckComponent->SetWorldLocation(CandidateTeleportLocation);
				TeleportPuckComponent->SetVisibility(true);				

				CandidateTeleportLocation.Z = 30.0f; // Hardcode floor height - good idea?!
			}
			else
			{
				CandidateTeleportLocation = FVector::ZeroVector;
				TeleportTubeMaterial->SetScalarParameterValue(NAME_Opacity, 0.01f);
				TeleportPuckComponent->SetVisibility(false);
			}
		}
	}
	else
	{
		// If we have a teleport spline mesh then destroy it
		if (TeleportSplineMeshActor)
		{
			TeleportSplineMeshActor->Destroy();
			TeleportSplineMeshActor = nullptr;
		}

		// If we have a teleport puckthen destroy it
		if (TeleportPuckComponent)
		{
			TeleportPuckComponent->DestroyComponent();
			TeleportPuckComponent = nullptr;
		}
	}
}