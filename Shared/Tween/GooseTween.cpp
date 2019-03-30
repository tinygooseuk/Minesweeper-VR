#include "GooseTween.h"

FGooseTweenBase::FGooseTweenBase(float InDuration)
	: CurrentTime(0.0f)
	, Duration(InDuration)
	, EasingFunc(nullptr)
	, EaseExp(2.0f)
{
	
}

void FGooseTweenBase::Tick(float DeltaSeconds)
{
	CurrentTime += DeltaSeconds; 
}

float FGooseTweenBase::GetProgress() const { 
	float OriginalProgress = CurrentTime / Duration; 
	
	return EasingFunc ? EasingFunc(OriginalProgress) : OriginalProgress;
}

bool FGooseTweenBase::IsComplete() const 
{
	return (CurrentTime / Duration) >= 1.0f;
}

FGooseTweenBase* FGooseTweenBase::ApplyEasing(AHEasing::AHEasingFunction InEasing, float InExp)
{
	EasingFunc = InEasing;
	EaseExp = InExp;

	return this;
}