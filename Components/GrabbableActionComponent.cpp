// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "GrabbableActionComponent.h"

// Engine
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Game
#include "System/MinesweeperMotionController.h"

UGrabbableActionComponent::UGrabbableActionComponent()
	: SphereRadius(100.0f)
	, SphereComponent(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabbableActionComponent::BeginPlay()
{
	Super::BeginPlay();
		
	// Create sphere 
	CreateSphereComponent();
}

#if WITH_EDITOR
void UGrabbableActionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!SphereComponent)
	{
		CreateSphereComponent();
	}
	else
	{
		SphereComponent->SetRelativeLocation(SphereLocation);
		SphereComponent->SetSphereRadius(SphereRadius);
	}
}
#endif

void UGrabbableActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TrackedController.IsValid())
	{
		bool bPressed = TrackedController->IsTriggerHeld() || TrackedController->IsGripHeld();
		if (bPressed)
		{
			OnInteracted.Broadcast();
		}
	}
}


void UGrabbableActionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMinesweeperMotionController* MotionController = Cast<AMinesweeperMotionController>(OtherActor);
	if (!MotionController)
	{
		return;
	}

	TrackedController = MotionController;	
}
void UGrabbableActionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMinesweeperMotionController* MotionController = Cast<AMinesweeperMotionController>(OtherActor);
	if (!MotionController)
	{
		return;
	}

	TrackedController = nullptr;
}

void UGrabbableActionComponent::CreateSphereComponent()
{
	if (SphereComponent)
	{
		return;
	}

	static FName NAME_OverlapVRController = TEXT("OverlapVRController");
	
	SphereComponent = NewObject<USphereComponent>(GetOwner());
	SphereComponent->InitSphereRadius(SphereRadius);
	SphereComponent->SetRelativeLocation(SphereLocation);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionProfileName(NAME_OverlapVRController);
	SphereComponent->bVisible = true;
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &UGrabbableActionComponent::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &UGrabbableActionComponent::OnEndOverlap);
	SphereComponent->SetupAttachment(GetOwner()->GetRootComponent());	

	SphereComponent->RegisterComponent();
}