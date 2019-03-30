// (c) 2017 TinyGoose Ltd., All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "GooseResourceMacros.h"

// The donkey-worker - don't call this yourself!!
bool __InternalGooseCheckFail(FString const &FinalMessage);

// Assertion Macros
//joejoejoe: TODO: Make work on non-Windows!
#if UE_BUILD_SHIPPING || !defined(_MSC_VER)
#define GOOSE_CHECK(x)					
#define GOOSE_CHECKF(x, msg, ...)		
#define GOOSE_VERIFY(x)								(x)
#define GOOSE_VERIFYF(x, msg, ...)					(x)
// Trace current method
#define GOOSE_TRACE()
#define GOOSE_NET_TRACE()                           
#else
#define GOOSE_CHECK(x)                              if (!!!(x)) { __InternalGooseCheckFail(FString::Printf(TEXT("%s:%i"), TEXT(__FUNCSIG__), __LINE__)); }
#define GOOSE_CHECKF(x, msg, ...)                   if (!!!(x)) { __InternalGooseCheckFail(FString::Printf(msg, __VA_ARGS__));  }
#define GOOSE_VERIFY(x)                             ((x) || __InternalGooseCheckFail(FString::Printf(TEXT("%s:%i"), TEXT(__FUNCSIG__), __LINE__)))
#define GOOSE_VERIFYF(x, msg, ...)                  ((x) || __InternalGooseCheckFail(FString::Printf(msg, __VA_ARGS__)))
// Trace current method
#define GOOSE_TRACE()                                FMsg::Logf(nullptr, 0, TEXT("GOOSE_TRACE"), ELogVerbosity::Display, *FString::Printf(TEXT("%s [%i]"), TEXT(__FUNCTION__), __LINE__))
#define GOOSE_NET_TRACE()                            FMsg::Logf(nullptr, 0, TEXT("GOOSE_TRACE"), ELogVerbosity::Display, *FString::Printf(TEXT("[%c] %s [%i]"), HasAuthority() ? TEXT('S') : TEXT('C'), TEXT(__FUNCTION__), __LINE__))
#endif

// GOOSE_BAIL with a standard return
#define GOOSE_BAIL(x)								if (!GOOSE_VERIFY((x)))							{ return; }
#define GOOSE_BAILF(x, msg, ...)					if (!GOOSE_VERIFYF((x), (msg), __VA_ARGS__))	{ return; }

// Generic GOOSE_BAIL with a code handler
#define GOOSE_BAIL_CODE(x, code)					if (!GOOSE_VERIFY((x)))							{ code; }

// Pre-made code handlers for returning a value, breaking, etc.
#define GOOSE_BAIL_CONTINUE(x)						GOOSE_BAIL_CODE(x, continue)
#define GOOSE_BAIL_BREAK(x)							GOOSE_BAIL_CODE(x, break)
#define GOOSE_BAIL_RETURN(x, retVal)				GOOSE_BAIL_CODE(x, return (retVal))

// Print debug messages (without GEngine!)
void __InternalDebugMessage(const FString& FinalString);

#define __DEBUG_FMT(Name, Fmt)	__InternalDebugMessage(FString::Printf(TEXT("%s = ") TEXT(Fmt), TEXT(#Name), Name));
#define DEBUG_FLOAT(Name)		__DEBUG_FMT(Name, "%f")
#define DEBUG_STRING(Name)		__DEBUG_FMT(Name, "%s")
#define DEBUG_INT(Name)			__DEBUG_FMT(Name, "%d")
#define DEBUG_HEX(Name)			__DEBUG_FMT(Name, "0x%x")

#define DEBUG_BOOL(Name)		__InternalDebugMessage(FString::Printf(TEXT("%s = %s"), TEXT(#Name), Name ? TEXT("true") : TEXT("false")));
#define DEBUG_OBJECT(Name)		__InternalDebugMessage(FString::Printf(TEXT("%s = %s"), TEXT(#Name), *Name.ToString()));
