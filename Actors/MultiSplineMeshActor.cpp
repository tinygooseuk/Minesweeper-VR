// (c) 2016-7 TinyGoose Ltd., All Rights Reserved.

#include "MultiSplineMeshActor.h"

#include "Components/SplineComponent.h"

AMultiSplineMeshActor::AMultiSplineMeshActor()
{
	USceneComponent *SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SceneRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(SceneRoot);

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	Spline->SetMobility(EComponentMobility::Movable);
	Spline->SetupAttachment(SceneRoot);
}

void AMultiSplineMeshActor::OnConstruction(FTransform const &MyTransform)
{
	Super::OnConstruction(MyTransform);
	RegenerateSpline();
}

void AMultiSplineMeshActor::RegenerateSpline()
{
	RemoveExistingSplineComponents();

	// Look out for mod 0 with no points D:
	uint32 NumSplinePoints = Spline->GetNumberOfSplinePoints();

	if (NumSplinePoints == 0)
	{
		return;
	}
	
	uint32 EndIndex = NumSplinePoints - 1;
	if (bClosedLoop)
	{
		// Do one more point if it's a closed loop
		EndIndex++; 
	}
	
	for (uint32 SplinePointIdx = 0; SplinePointIdx < EndIndex; ++SplinePointIdx)
	{
		// Work out working points
		uint32 Point1Idx = SplinePointIdx;
		uint32 Point2Idx = (SplinePointIdx + 1) % NumSplinePoints; // Maybe wrap

		// Get points and tangents
		FVector Point1Pos, Point1Tng;
		Spline->GetLocationAndTangentAtSplinePoint(Point1Idx, Point1Pos, Point1Tng, ESplineCoordinateSpace::World);

		FVector Point2Pos, Point2Tng;
		Spline->GetLocationAndTangentAtSplinePoint(Point2Idx, Point2Pos, Point2Tng, ESplineCoordinateSpace::World);

		// Calculate midpoint
		FVector SplineSectionMidPoint = (Point1Pos + Point2Pos) / 2;

		// Create the spline mesh!
		USplineMeshComponent *NewSection = NewObject<USplineMeshComponent>(this);
		NewSection->SetMobility(EComponentMobility::Movable);
		NewSection->RegisterComponent();
		NewSection->SetRelativeLocation(SplineSectionMidPoint);
		NewSection->SetStaticMesh(StaticMesh);

		if (OverrideMaterial)
		{
			NewSection->SetMaterial(0, OverrideMaterial);
		}

		// Configure it
		NewSection->SetStartAndEnd(Point1Pos - SplineSectionMidPoint, Point1Tng, Point2Pos - SplineSectionMidPoint, Point2Tng, false);
		NewSection->SetSplineUpDir(UpDirection, false);
		NewSection->SetForwardAxis(ForwardAxis.GetValue(), true);
		NewSection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		NewSection->SetCollisionProfileName(CollisionProfile.Name);
		NewSection->bCastDynamicShadow = bCastShadow;
		NewSection->bCastStaticShadow = bCastShadow;
		NewSection->SetGenerateOverlapEvents(CollisionProfile.Name.ToString().Contains(TEXT("overlap")));
		NewSection->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	}
}

USplineComponent *AMultiSplineMeshActor::GetSpline() const
{
	return Spline;
}

void AMultiSplineMeshActor::SetPoints(TArray<FVector> const &Points)
{
	GOOSE_CHECK(Spline);

	Spline->ClearSplinePoints();

	for (FVector const &Point : Points)
	{
		Spline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
	}

	Spline->UpdateSpline();
	RegenerateSpline();
}

void AMultiSplineMeshActor::RemoveExistingSplineComponents()
{
	// Remove all existing spline components
	for (UActorComponent *ActorComp : GetComponentsByClass(USplineMeshComponent::StaticClass()))
	{
		ActorComp->DestroyComponent();
	}
}
