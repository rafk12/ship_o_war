// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "SocketWrapper.generated.h"

UENUM(BlueprintType)
enum class ESocketEnum : uint8
{
	SE_Connected UMETA(DisplayName = "Connected"),
	SE_Disconnected UMETA(DisplayName = "Disconnected")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketStatus, ESocketEnum, Status);

/**
 * 
 */
UCLASS(BlueprintType)
class SHIP_O_WAR_API USocketWrapper : public UObject
{
	GENERATED_BODY()
public:
	FSocket* socket;

	FSocketStatus SocketStatus_Updated;

	USocketWrapper();

	void Initialize(const FString& name, bool udp = false);

	void BeginDestroy() override;
};
