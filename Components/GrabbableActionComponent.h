// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "Minesweeper_VR.h"
#include "Components/ActorComponent.h"
#include "GrabbableActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGrabbableGrabbedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINESWEEPER_VR_API UGrabbableActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGrabbableActionComponent();

	// AActor:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	///////////////////////// Events /////////////////////////
	// On interacted callback
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "VR Interaction")
	FGrabbableGrabbedSignature OnInteracted;

	///////////////////////// Properties /////////////////////////
	// Sphere radius in metres
	UPROPERTY(EditAnywhere)
	float SphereRadius;

	// Sphere location
	UPROPERTY(EditAnywhere)
	FVector SphereLocation;
	
private:
	// Create a sphere component if none existing
	void CreateSphereComponent();

	///////////////////////// State /////////////////////////
	// The tracked controller, if any
	UPROPERTY()
	TWeakObjectPtr<class AMinesweeperMotionController> TrackedController;

	///////////////////////// Components /////////////////////////
	// The sphere component
	UPROPERTY()
	class USphereComponent* SphereComponent;
};
