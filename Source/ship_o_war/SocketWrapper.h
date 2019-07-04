// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "SocketWrapper.generated.h"

UENUM(BlueprintType)
enum class EBattleshipSocketConnectionState : uint8
{
	BPSocket_NotConnected UMETA(DisplayName="Not connected"),
	BPSocket_Connected UMETA(DisplayName="Connected"),
	BPSocket_ConnectionError UMETA(DisplayName="Connection error")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketConnectionStateDelegate, EBattleshipSocketConnectionState, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSCreateGridDelegate, int32, Width, int32, Height);

/**
 * 
 */
UCLASS(BlueprintType) class SHIP_O_WAR_API UBattleShipSocketWrapper : public UObject
{
	GENERATED_BODY()

	void Tick();

	FSocket* Socket;
	ESocketConnectionState LastConnectionState;
public:

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket", meta=(DisplayName="Create new socket", WorldContext = WorldContextObject))
	static UBattleShipSocketWrapper* Create(UObject* WorldContextObject);

	void Init();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Networking|Battleship|Socket")
	bool Connect(const FString& IP, int32 Port) const;

	UPROPERTY(BlueprintAssignable)
	FSocketConnectionStateDelegate SocketConnectionStateChanged;

	//UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Networking|Socket")
	//int32 Send(const TArray<uint8>& bytes) const;

	void BeginDestroy() override;
};
