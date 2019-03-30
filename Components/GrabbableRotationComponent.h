// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "Components/ActorComponent.h"
#include "GrabbableRotationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINESWEEPER_VR_API UGrabbableRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// AActor:
	UGrabbableRotationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///////////////////////// Public functions /////////////////////////
	// Add object which will contribute to rotation changes
	void AddRotator(class AMinesweeperMotionController* Controller);
	// Remove object from contributing to rotation changes
	void RemoveRotator(class AMinesweeperMotionController* Controller);

	///////////////////////// Public components /////////////////////////
	// The component we should be transforming (or root)
	UPROPERTY(EditAnywhere)
	class UPrimitiveComponent* ComponentToTransform;

private:
	///////////////////////// Functions /////////////////////////
	// Begin a new rotation
	void BeginRotations();
	// Commit the in-flight rotation and add any spin required
	void CommitRotations();
	// Apply rotators to rotation
	void ApplyRotators();
	// Get the current normal of the rotators
	FVector GetRotatorsNormal() const;

	///////////////////////// State /////////////////////////
	// The list of controllers which are acting upon us
	TArray<class AMinesweeperMotionController*> RotatorActors;

	// Initial transform
	FTransform InitialComponentTransform;
	// Initial normal
	FVector InitialRotatorsNormal;
	// Last known rotator normal
	FVector LastRotatorsNormal;
	// One before last known rotator normal (for calculating spin)
	FVector PenultimateRotatorsNormal;
};
