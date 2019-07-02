// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SocketWrapper.h"
#include "SocketInteropLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SHIP_O_WAR_API USocketInteropLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Networking|Socket")
	static USocketWrapper* CreateSocket(UObject * outer, const FString& name, bool udp = false);
	
};
