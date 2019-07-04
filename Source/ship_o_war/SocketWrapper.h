// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "ENgine/EngineTypes.h"
#include "TimerManager.h"
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

	union FByteData
	{
		uint8 Uns;
		int8 sig;
	};

	enum class EState : uint8
	{
		None = 0,
		Grid = 1,
		Skin = 2,
		Slot = 3,
		Win = 4
	};

	int32 CurrentStateData;
	EState CurrentState;

	FTimerHandle Timer;
	FSocket* Socket;

	void HandleByte(FByteData Data);

	void Tick();
public:

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket", meta=(DisplayName="Create new socket",
		WorldContext = WorldContextObject))
	static UBattleShipSocketWrapper* Create(UObject* WorldContextObject);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (DisplayName = "Connection state"))
	ESocketConnectionState LastConnectionState;

	UPROPERTY(BlueprintAssignable)
	FSocketConnectionStateDelegate SocketConnectionStateChanged;

	UPROPERTY(BlueprintAssignable)
	FBSCreateGridDelegate BSCreateGridEvent;

	void Init();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Networking|Battleship|Socket")
	bool Connect(const FString& IP, int32 Port);

	//UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Networking|Socket")
	//int32 Send(const TArray<uint8>& bytes) const;

	void BeginDestroy() override;
};
