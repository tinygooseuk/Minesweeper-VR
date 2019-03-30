#include "GooseUtil.h"

// Engine
#include "Camera/CameraShake.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundWave.h"
#include "TimerManager.h"
#include "UObject/Object.h"

void UGooseUtil::DoNextTick(UObject *WorldContextObject, TFunction<void()> const &Do)
{
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda(Do);

	WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(TimerCallback);
}

void UGooseUtil::DoAfter(UObject *WorldContextObject, float Timeout, TFunction<void()> const &Do)
{
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda(Do);

	FTimerHandle Handle;
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, Timeout, false);
}

UClass *UGooseUtil::GetClass(FString const &ClassName)
{
	int32 LastSlash;
	ClassName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = ClassName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("Class'/Game/%s.%s_C'"), *ClassName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UClass>(FName(*FullName));
}
UClass *UGooseUtil::GetClass(FString const &ClassName, FString const &Folder)
{
	int32 LastSlash;
	ClassName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = ClassName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("Class'/Game/%s/%s.%s_C'"), *Folder, *ClassName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UClass>(FName(*FullName));
}
UClass *UGooseUtil::GetBPClass(FString const &ClassName)
{
	return GetClass(ClassName, TEXT("Blueprints"));
}
UClass *UGooseUtil::GetUIClass(FString const &ClassName)
{
	return GetClass(ClassName, TEXT("UI"));
}
UMaterial *UGooseUtil::GetMaterial(FString const &MatName)
{
	int32 LastSlash;
	MatName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = MatName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("Material'/Game/Materials/%s.%s'"), *MatName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UMaterial>(*FullName);
}
UMaterialInstanceConstant *UGooseUtil::GetMaterialInstance(FString const &MIName)
{
	int32 LastSlash;
	MIName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = MIName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("MaterialInstanceConstant'/Game/Materials/%s.%s'"), *MIName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UMaterialInstanceConstant>(*FullName);
}
UPhysicalMaterial *UGooseUtil::GetPhysicalMaterial(FString const &MatName)
{
	int32 LastSlash;
	MatName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = MatName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("PhysicalMaterial'/Game/PhysicsMaterials/%s.%s'"), *MatName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UPhysicalMaterial>(*FullName);
}
UTexture *UGooseUtil::GetTexture(FString const &TexName)
{
	int32 LastSlash;
	TexName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = TexName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("Texture'/Game/Textures/%s.%s'"), *TexName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UTexture>(*FullName);
}
UStaticMesh *UGooseUtil::GetStaticMesh(FString const &MeshName)
{
	int32 LastSlash;
	MeshName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = MeshName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("StaticMesh'/Game/StaticMeshes/%s.%s'"), *MeshName, *ShortName);

	return UGooseUtil::LoadObjFromPath<UStaticMesh>(*FullName);
}
USoundWave *UGooseUtil::GetSoundWave(FString const &SoundName)
{
	int32 LastSlash;
	SoundName.FindLastChar(TEXT('/'), LastSlash);

	FString ShortName = SoundName.Mid(LastSlash + 1);
	FString FullName = FString::Printf(TEXT("SoundWave'/Game/Sounds/%s.%s'"), *SoundName, *ShortName);

	return UGooseUtil::LoadObjFromPath<USoundWave>(*FullName);
}

bool UGooseUtil::IsPIE(UObject *WorldContextObject) 
{
#if WITH_EDITOR
    if (!GOOSE_VERIFY(WorldContextObject && WorldContextObject->GetWorld())) 
	{
        return true; // Damn, this is bad - assume we're in PIE though for now
    }
    
	return WorldContextObject->GetWorld()->WorldType == EWorldType::PIE;
#else
	return false;
#endif
}

AActor *UGooseUtil::CloneActor(AActor *Template)
{
	if (!GOOSE_VERIFYF(Template, TEXT("Invalid/null template passed to UGooseUtil::CloneActor()")))
	{
		return nullptr;
	}

	UWorld *World = Template->GetWorld();
	GOOSE_CHECK(World);
	
	FTransform InitialTransform = Template->GetTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Template = Template;
	SpawnParams.bNoFail = true;
	SpawnParams.Name = "t";

	return World->SpawnActor(Template->GetClass(), &InitialTransform, SpawnParams);
}

FName UGooseUtil::BreakCollisionProfileName(FCollisionProfileName CollisionProfileName)
{
	return CollisionProfileName.Name;
}