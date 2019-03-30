// (c) 2017 TinyGoose Ltd., All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "Easing.h"
//#include "Kismet/KismetMathLibrary.h"

class FGooseTweenBase
{
public:
	FGooseTweenBase(float InDuration);
	virtual ~FGooseTweenBase() { }

	virtual void BeginPlay() { }
	virtual void EndPlay() { }

	virtual void Tick(float DeltaSeconds);

	float GetProgress() const;
		
	virtual bool IsComplete() const;

	FGooseTweenBase* ApplyEasing(AHEasing::AHEasingFunction InEasingFunc, float InExp = 2.0f);
	
private:
	AHEasing::AHEasingFunction EasingFunc;
	float EaseExp;

	float CurrentTime;
	float Duration;
};
#define GT_Ease(Type, InnerTween)			((InnerTween)->ApplyEasing(AHEasing::Type))


///////////////////////// Basic Tweens /////////////////////////
template <typename T>
class TSimpleGooseTween : public FGooseTweenBase
{
public:
	TSimpleGooseTween(float InDuration, T InStartValue, T InEndValue, T& InVariable)
		: FGooseTweenBase(InDuration)
		, StartValue(InStartValue)
		, EndValue(InEndValue)
		, Variable(&InVariable) { 
	}

	virtual void Tick(float DeltaSeconds) override
	{
		FGooseTweenBase::Tick(DeltaSeconds);

		float Alpha = GetProgress();
		*Variable = FMath::Lerp(StartValue, EndValue, Alpha);
	}

	virtual void EndPlay() override
	{
		FGooseTweenBase::EndPlay();

		*Variable = EndValue;
	}

private:
	T* Variable;

	T StartValue;
	T EndValue;
};
#define GT_Simple(Duration, Start, End, Var)		new TSimpleGooseTween<decltype(Var)>(Duration, Start, End, Var)


template <typename T>
class TLambdaGooseTween : public FGooseTweenBase
{
public:
	TLambdaGooseTween(float InDuration, T InStartValue, T InEndValue, const TFunction<void(T)> &InLambda)
		: FGooseTweenBase(InDuration)
		, StartValue(InStartValue)
		, EndValue(InEndValue)
		, Lambda(InLambda) { 
	}

	virtual void Tick(float DeltaSeconds) override
	{
		FGooseTweenBase::Tick(DeltaSeconds);

		float Alpha = GetProgress();
		if (Lambda)
		{
			Lambda(FMath::Lerp(StartValue, EndValue, Alpha));
		}
	}

	virtual void EndPlay() override
	{
		FGooseTweenBase::EndPlay();

		if (Lambda)
		{
			Lambda(EndValue);
		}
	}

private:
	TFunction<void(T)> Lambda;

	T StartValue;
	T EndValue;
};
#define GT_Lambda(Duration, Start, End, Func)	new TLambdaGooseTween<decltype(Start)>(Duration, Start, End, Func)

/*

// Basics

template <typename T>
class USetterGooseTween : public UGooseTween
{
	// calls some object's SetXXX() method
};


// Containers
class USeriesGooseTween : public UGooseTween
{
	// runs multiple tweens in series
};
class UParallelGooseTween : public UGooseTween
{
	// runs multiple tweens in parallel
};
class URepeatNGooseTween : public UGooseTween
{
	// repeates tween N times
};
class URepeatForeverGooseTween : public UGooseTween
{
	// repeates tween forever
};




#define GOOSE_TWEEN_SERIES(...)
#define GOOSE_TWEEN_REPEAT_N(N, ...)
#define GOOSE_TWEEN_REPEAT_FOREVER(...)
#define GOOSE_TWEEN_PARALLEL(...)

#define GOOSE_TWEEN_EASE(Func, Root)

#define GOOSE_TWEEN_SETTER(Duration, Start, End, Obj, Func)
#define GOOSE_TWEEN_LAMBDA(Duration, Start, End, Lambda)

inline void Test()
{
	RunGooseTween
	(
		GOOSE_TWEEN_SERIES
		(
			GOOSE_TWEEN_EASE
			(
				EaseIn,
				GooseTweenSimple(1.0f, 0.0f, 1.0f, MyFloat)
			),
			GOOSE_TWEEN_LAMBDA
			(
				2.0f, 100.0f, 200.0f,
				[](float Alpha)
				{
					UE_LOG(LogTemp, Warning, TEXT("Alpha is %f"), Alpha);
				}
			),
			GOOSE_TWEEN_SETTER(1.0f, FVector::ZeroVector, FVector::OneVector, this, &AActor::SetActorLocation),
			GOOSE_TWEEN_REPEAT_N
			(
				5,
				GooseTweenSimple(0.5f, 0, 400, MyInt)
			)
		)
	);
}*/