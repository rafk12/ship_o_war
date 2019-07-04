// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPAddress.h"
#include "TimerManager.h"
#include "SocketInteropLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHostResolveStatus, int32, ErrorCode, const FString&, Address);

UCLASS() class SHIP_O_WAR_API UHostResolveInfo : public UObject
{
	GENERATED_BODY()

	FResolveInfo* Info;
	FTimerHandle Timer;
	FHostResolveStatus Callback;

	void ResolveCheckLoop();

public:

	UFUNCTION(BlueprintCallable, Category = "Networking|IP", meta = (DisplayName = "Create host name resolver", WorldContext = WorldContextObject, AutoCreateRefTerm = "OnComplete"))
	static UHostResolveInfo* Create(UObject* WorldContextObject, const FString& name, const FHostResolveStatus& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Networking|IP")
	bool IsComplete() const;

	UFUNCTION(BlueprintCallable, Category = "Networking|IP")
	int32 GetErrorCode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Networking|IP")
	FString GetResolvedAddress() const;

};
