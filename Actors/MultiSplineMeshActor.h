// (c) 2016-7 TinyGoose Ltd., All Rights Reserved.
// This code shamelessly ripped from older project ;)

#pragma once
#include "Minesweeper_VR.h"

#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/CollisionProfile.h"

#include "MultiSplineMeshActor.generated.h"

UCLASS()
class MINESWEEPER_VR_API AMultiSplineMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMultiSplineMeshActor();
	
	///// Overrides //////////////////////////////////////////////////
	void OnConstruction(FTransform const &MyTransform) override;

	///// Public functions ///////////////////////////////////////////
	// Regenerates the spline with any new points required
	void RegenerateSpline();

	// Getter for Spline member	
	class USplineComponent *GetSpline() const;

	// Set all points
	void SetPoints(TArray<FVector> const &Points);

	///// Members ////////////////////////////////////////////////////
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	class UStaticMesh *StaticMesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	class UMaterialInterface *OverrideMaterial = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	bool bClosedLoop = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	bool bCastShadow = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	FVector UpDirection = FVector::UpVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis = ESplineMeshAxis::Y;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MultiSpline Mesh")
	FCollisionProfileName CollisionProfile;

private:
	void RemoveExistingSplineComponents();

	UPROPERTY() class USplineComponent *Spline;
};
