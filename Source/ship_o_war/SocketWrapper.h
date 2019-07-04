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
	BPSocket_NotConnected  = ESocketConnectionState::SCS_NotConnected UMETA(DisplayName="Not connected"),
	BPSocket_Connected = ESocketConnectionState::SCS_Connected UMETA(DisplayName="Connected"),
	BPSocket_ConnectionError = ESocketConnectionState::SCS_ConnectionError UMETA(DisplayName="Connection error")
};

UENUM(BlueprintType)
	enum class EBattleshipMapSlotState : uint8
{
	BPMap_Intact UMETA(DisplayName="Intact"),
	BPMap_Smoke UMETA(DisplayName="Smoke"),
	BPMap_Fire UMETA(DisplayName="Fire"),
	BPMap_Explore UMETA(DisplayName="Explode")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketConnectionStateDelegate, EBattleshipSocketConnectionState, Status);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSCreateGridDelegate, int32, Width, int32, Height);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSSelectSkinDelegate, int32, Code);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FBSRevealSlotDelegate, int32, X, int32, Y, int32, Width, int32, Height,
                                              int32, EBattleshipMapSlotState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSWinDelegate, bool, Me);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSTurnDelegate, bool, Me);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSBoatCountDelegate, int32, BoatCount);

/**
 * 
 */
UCLASS(BlueprintType) class SHIP_O_WAR_API UBattleShipSocketWrapper : public UObject
{
	GENERATED_BODY()

	union FByteData
	{
		uint8 Uns;
		int8 Sig;
	};

	enum class EReadingState : uint8
	{
		None = 0,
		Grid = 1,
		Skin = 2,
		Slot = 3,
		Win = 4,
		BoatCount = 5,
		Turn = 6
	};

	enum class EWriteValues : uint8
	{
		Skin = 0,
		Click = 1
	};

	TArray<int32> CurrentStateData;
	EReadingState CurrentState;

	FTimerHandle Timer;
	FSocket* Socket;

	void Init();

	void HandleByte(FByteData Data);

	void Tick();

	bool SocketConnectionGuarantee() const;
public:

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket", meta=(DisplayName="Create new socket",
		WorldContext = WorldContextObject))
	static UBattleShipSocketWrapper* Create(UObject* WorldContextObject);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (DisplayName = "Connection state"))
	EBattleshipSocketConnectionState LastConnectionState;

	UPROPERTY(BlueprintAssignable)
	FSocketConnectionStateDelegate SocketConnectionStateChanged;

	UPROPERTY(BlueprintAssignable)
	FBSCreateGridDelegate BSCreateGridEvent;

	UPROPERTY(BlueprintAssignable)
	FBSSelectSkinDelegate BSSelectSkinEvent;

	UPROPERTY(BlueprintAssignable)
	FBSRevealSlotDelegate BSRevealSlotEvent;

	UPROPERTY(BlueprintAssignable)
	FBSWinDelegate BSWinEvent;

	UPROPERTY(BlueprintAssignable)
	FBSBoatCountDelegate BSBoatCountEvent;

	UPROPERTY(BlueprintAssignable)
	FBSTurnDelegate BSTurnEvent;

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket")
	bool Connect(const FString& IP, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket")
	void Disconnect(const bool Recreate = true);

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket")
	void SendSkin(const int32 Skin);

	UFUNCTION(BlueprintCallable, Category = "Networking|Battleship|Socket")
	void SendClick(const int32 X, const int32 Y);

	void BeginDestroy() override;
};
