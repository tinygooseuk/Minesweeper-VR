#pragma once
#include "ConstructorHelpers.h"

// Find a resource/engine resource - ctor ONLY!
#define FIND_ENGINE_RESOURCE(Type, Name, Path)	ConstructorHelpers::FObjectFinder<U##Type>(TEXT(#Type) TEXT("'/Engine/")	Path TEXT("/") TEXT(#Name) TEXT(".") TEXT(#Name) TEXT("'")).Object;
#define FIND_RESOURCE(Type, Name, Path)			ConstructorHelpers::FObjectFinder<U##Type>(TEXT(#Type) TEXT("'/Game/")		Path TEXT("/") TEXT(#Name) TEXT(".") TEXT(#Name) TEXT("'")).Object;

// Find a class/engine class - ctor ONLY
#define FIND_ENGINE_CLASS(Name, Path)			ConstructorHelpers::FObjectFinder<UClass>(TEXT("Class'/Engine/")			Path TEXT("/") TEXT(#Name) TEXT(".") TEXT(#Name) TEXT("_C'")).Object;
#define FIND_CLASS(Name, Path)					ConstructorHelpers::FObjectFinder<UClass>(TEXT("Class'/Game/")				Path TEXT("/") TEXT(#Name) TEXT(".") TEXT(#Name) TEXT("_C'")).Object;

// Define a resource struct, F<class>Resources
#define BEGIN_RESOURCES(ClassName)				struct F##ClassName##Resources \
												{
// Add resources to a struct as above
#define ADD_ENGINE_RESOURCE(Type, Name, Path)		U##Type* Name = FIND_ENGINE_RESOURCE(Type, Name, Path);
#define ADD_RESOURCE(Type, Name, Path)				U##Type* Name = FIND_RESOURCE(Type, Name, Path);

// Add classes to a struct
#define ADD_ENGINE_CLASS(Name, Path)				UClass* Name = FIND_ENGINE_CLASS(Name, Path);
#define ADD_CLASS(Name, Path)						UClass* Name = FIND_CLASS(Name, Path);

// Close out the resource struct
#define END_RESOURCES(ClassName)				} ClassName##Resources;

// Keep a resource(s)
#define KEEP_RESOURCE(ClassName, Name)			KEEP_RESOURCE_EX(ClassName, Name, Name)
#define KEEP_RESOURCE_EX(ClassName, Name, Var)	Var = ClassName##Resources.Name