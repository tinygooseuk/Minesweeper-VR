// (c) 2016-7 TinyGoose Ltd., All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GooseMacros.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/CollisionProfile.h"

#include "GooseUtil.generated.h"

UCLASS()
class UGooseUtil : public UBlueprintFunctionLibrary 
{
	GENERATED_BODY()

public:
	/* 
		Timing Functions
	*/
	// Defer an action until the next Tick()
	static void DoNextTick(UObject *WorldContextObject, TFunction<void()> const &Do);
	// Defer an action until a given timeout
	static void DoAfter(UObject *WorldContextObject, float Timeout, TFunction<void()> const &Do);


	/* 
		Class/object/resource finders
	*/
	// Get a blueprint class, i.e. "Characters/PlayerCharacter_BP"
	static UClass *GetClass(FString const &ClassName);
	static UClass *GetClass(FString const &ClassName, FString const &Folder);
	// Get a blueprint class, i.e. "Characters/PlayerCharacter_BP"
	static UClass *GetBPClass(FString const &ClassName);
	// Get a blueprint class, i.e. "Characters/PlayerCharacter_BP"
	static UClass *GetUIClass(FString const &ClassName);
	// Get a material
	static class UMaterial *GetMaterial(FString const &MatName);
	// Get a material instance
	static class UMaterialInstanceConstant *GetMaterialInstance(FString const &MIName);
	// Get a physical material
	static class UPhysicalMaterial *GetPhysicalMaterial(FString const &MatName);
	// Get a material
	static class UTexture *GetTexture(FString const &TexName);
	// Get a static mesh
	static class UStaticMesh *GetStaticMesh(FString const &MeshName);
	// Get a sound
	static class USoundWave *GetSoundWave(FString const &SoundName);

	// Get any object from a name and folder
    template <typename T>
    static T *GetObject(FString const &Object, FString const &Folder);

	/*
		Misc. Editor/BP things
	*/
	// Determine if running in Play-In-Editor mode
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor")
	static bool IsPIE(UObject *WorldContextObject);

	// Clone an Actor subclass
	template <typename ActorClass>
	static ActorClass *CloneActor(ActorClass *Template) 
	{
		return (ActorClass *)CloneActor((AActor *)Template);
	}

	UFUNCTION(BlueprintCallable, Category = "Actor")
	static AActor *CloneActor(AActor *Template);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Collisions")
	static FName BreakCollisionProfileName(FCollisionProfileName CollisionProfileName);
    
private:
	// Load an object dynamically, given a path
	template <typename T>
	static FORCEINLINE T *LoadObjFromPath(FName const &Path) 
	{
		if (Path == NAME_None) return nullptr;

		return Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *Path.ToString()));
	}
};

// Implementation of GetObject
template <typename T>
inline T *UGooseUtil::GetObject(FString const &Object, FString const &Folder)
{
    FString FullPath = FString::Printf(TEXT("/Game/%s/%s.%s"), *Folder, *Object, *Object);
    return (T *)UGooseUtil::LoadObjFromPath<T>(FName(*FullPath));
}

// Getting a UClass works a bit differently, so defer to that logic here
template <>
inline UClass *UGooseUtil::GetObject(FString const &Object, FString const &Folder)
{
    return UGooseUtil::GetClass(Object, Folder);
}

