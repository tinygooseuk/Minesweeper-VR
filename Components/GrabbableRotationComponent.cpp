// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "GrabbableRotationComponent.h"

// Engine
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

// Game
#include "System/MinesweeperMotionController.h"

UGrabbableRotationComponent::UGrabbableRotationComponent()
	: ComponentToTransform(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGrabbableRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	// If no component to transform, find a primitive component
	if (!ComponentToTransform)
	{
		AActor* Owner = GetOwner();
		GOOSE_BAIL(Owner);

		ComponentToTransform = Owner->FindComponentByClass<UPrimitiveComponent>();
	}
}


void UGrabbableRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyRotators();
}


void UGrabbableRotationComponent::AddRotator(AMinesweeperMotionController* Controller)
{
	GOOSE_BAIL(Controller);
	GOOSE_BAIL(!RotatorActors.Contains(Controller));

	RotatorActors.Add(Controller);

	// Not first rotator? Restart rotations
	if (RotatorActors.Num() > 1) 
	{
		CommitRotations();
	}

	BeginRotations();
}

void UGrabbableRotationComponent::RemoveRotator(AMinesweeperMotionController* Controller)
{
	GOOSE_BAIL(Controller);
	GOOSE_BAIL(RotatorActors.Contains(Controller));

	CommitRotations();

	RotatorActors.Remove(Controller);

	// If some rotators still remain, begin rotations again
	if (RotatorActors.Num() > 0)
	{
		BeginRotations();
	}
}

void UGrabbableRotationComponent::BeginRotations()
{
	if (!GOOSE_VERIFY(ComponentToTransform))
	{
		return;
	}

	// Store current rotations
	InitialComponentTransform = ComponentToTransform->GetComponentTransform();
	InitialRotatorsNormal = GetRotatorsNormal();
	LastRotatorsNormal = InitialRotatorsNormal;

	// Stop spinning
	ComponentToTransform->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}
void UGrabbableRotationComponent::CommitRotations()
{
	//FVector FinalDelta = (PenultimateRotatorsPosition - LastRotatorsPosition) * 100.0f;
	FQuat Torque = FQuat::FindBetweenNormals(LastRotatorsNormal, PenultimateRotatorsNormal);
	
	FVector Axis = Torque.GetRotationAxis();
	float Angle = FMath::RadiansToDegrees(Torque.GetAngle());
	
	ComponentToTransform->AddAngularImpulseInDegrees(Axis * Angle * -100.0f, NAME_None, true);
}

void UGrabbableRotationComponent::ApplyRotators()
{
	if (RotatorActors.Num() == 0 || !GOOSE_VERIFY(ComponentToTransform))
	{
		return;
	}

	// Apply rotation
	FVector CurrentRotatorsNormal = GetRotatorsNormal();

	FQuat DeltaQuat = FQuat::FindBetweenNormals(InitialRotatorsNormal, CurrentRotatorsNormal);

	ComponentToTransform->SetWorldRotation(DeltaQuat * InitialComponentTransform.GetRotation());
	PenultimateRotatorsNormal = LastRotatorsNormal;
	LastRotatorsNormal = CurrentRotatorsNormal;
}

FVector UGrabbableRotationComponent::GetRotatorsNormal() const
{
	FVector AllNormals = FVector::ZeroVector;

	AActor* Owner = GetOwner();
	GOOSE_BAIL_RETURN(Owner, -FVector::ForwardVector); // default towards player?

	for (AActor* RotatorActor : RotatorActors)
	{
		FVector ThisNormal = RotatorActor->GetActorLocation() - Owner->GetActorLocation();
		ThisNormal.Normalize();
		

		AllNormals += ThisNormal;
	}

	AllNormals /= FVector(RotatorActors.Num()); 

	return AllNormals;
}