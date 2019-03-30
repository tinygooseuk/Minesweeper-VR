// (c) 2017 TinyGoose Ltd., All Rights Reserved.
#include "GooseMacros.h"

#include "Engine/Engine.h"

bool __InternalGooseCheckFail(FString const &FinalMessage)
{
	FMsg::Logf(nullptr, 0, TEXT("GOOSE_CHECK"), ELogVerbosity::Error, TEXT("%s"), *FinalMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GOOSE_CHECK Failed: %s"), *FinalMessage));

	UE_DEBUG_BREAK();
	return false;
}

void __InternalDebugMessage(const FString& FinalString)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FinalString);
}