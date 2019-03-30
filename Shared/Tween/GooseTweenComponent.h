// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GooseTween.h"
#include "Shared/Util/GooseMacros.h"
#include "GooseTweenComponent.generated.h"

UCLASS()
class UGooseTweenComponent : public UActorComponent
{
public:
	GENERATED_BODY()

	UGooseTweenComponent();
		
	void Init(FGooseTweenBase* InRootTween);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	FGooseTweenBase* RootTween;
};

#define RunGooseTween(Root)														\
	do 																				\
	{																				\
		UGooseTweenComponent* __Tween__ = NewObject<UGooseTweenComponent>(this);	\
		__Tween__->Init(Root);														\
		__Tween__->RegisterComponent();												\
	} while (false);