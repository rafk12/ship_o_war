// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketInteropLibrary.h"
#include "SocketSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ship_o_war.h"

void UHostResolveInfo::ResolveCheckLoop()
{
	if (!Info->IsComplete())
	{
		return;
	}
	const auto* World = GetWorld();
	if (IsValid(World))
	{
		World->GetTimerManager().ClearTimer(Timer);
		if (!Callback.ExecuteIfBound(GetErrorCode(), GetResolvedAddress()))
		{
			UE_LOG(LogBattleship, Error, TEXT("%s: Resolve callback could not be called"), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogBattleship, Error, TEXT("%s: Invalid world"), *GetNameSafe(this));
	}
}

UHostResolveInfo* UHostResolveInfo::Create(UObject* WorldContextObject, const FString& name,
                                           const FHostResolveStatus& OnComplete)
{
	const auto ObjName = FString::Printf(TEXT("UHostResolveInfo(%s)"), *name);
	auto* Obj = NewObject<UHostResolveInfo>(WorldContextObject, *ObjName);
	Obj->Info = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetHostByName(StringCast<ANSICHAR>(*name).Get());
	Obj->Callback = OnComplete;
	if (OnComplete.IsBound())
	{
		const auto* World = Obj->GetWorld();
		if (IsValid(World))
		{
			World->GetTimerManager().SetTimer(Obj->Timer, Obj, &UHostResolveInfo::ResolveCheckLoop, 1.0f, true);
		}
	}
	return Obj;
}

bool UHostResolveInfo::IsComplete() const
{
	return Info->IsComplete();
}

int32 UHostResolveInfo::GetErrorCode() const
{
	return Info->GetErrorCode();
}

FString UHostResolveInfo::GetResolvedAddress() const
{
	return Info->GetResolvedAddress().ToString(false);
}
