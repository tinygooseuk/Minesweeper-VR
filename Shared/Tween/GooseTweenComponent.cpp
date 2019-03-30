#include "GooseTweenComponent.h"

#include "GooseTween.h"

UGooseTweenComponent::UGooseTweenComponent()
	: RootTween(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}
	
void UGooseTweenComponent::Init(FGooseTweenBase* InRootTween)
{
	if (RootTween)
	{
		RootTween->EndPlay();
	}

	RootTween = InRootTween;
	RootTween->BeginPlay();
}


void UGooseTweenComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GOOSE_BAIL(RootTween);

	RootTween->Tick(DeltaTime);

	if (RootTween->IsComplete())
	{
		RootTween->EndPlay();
		DestroyComponent();
	}
}
