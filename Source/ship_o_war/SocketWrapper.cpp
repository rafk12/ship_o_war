// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketWrapper.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "ship_o_war.h"
#include "Engine/World.h"

UBattleShipSocketWrapper* UBattleShipSocketWrapper::Create(UObject* WorldContextObject)
{
	auto* Obj = NewObject<UBattleShipSocketWrapper>(WorldContextObject, TEXT("UBattleShipSocketWrapper"));
	Obj->Init();
	return Obj;
}

void UBattleShipSocketWrapper::Init()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
		CreateSocket(NAME_Stream, TEXT("BattleshipSocket"), false);
	LastConnectionState = SCS_NotConnected;
}

void UBattleShipSocketWrapper::HandleByte(FByteData Data)
{
	switch (CurrentState)
	{
	case EState::None:
		{
			CurrentState = static_cast<EState>(Data.Uns);
			CurrentStateData = -1;
		}
		break;
	case EState::Grid:
		{
			if(CurrentStateData == -1)
			{
				CurrentStateData = Data.sig;
				return;
			}
			BSCreateGridEvent.Broadcast(CurrentStateData, Data.sig);
			CurrentState = EState::None;
		}
		break;
	case EState::Skin:
		{
		}
		break;
	case EState::Slot:
		{
		}
		break;
	case EState::Win:
		{
		}
		break;
	default:
		{
			CurrentState = EState::None;
		};
	}
}

void UBattleShipSocketWrapper::Tick()
{
	const auto ConnectionState = Socket->GetConnectionState();
	if (ConnectionState != LastConnectionState)
	{
		LastConnectionState = ConnectionState;
		EBattleshipSocketConnectionState state;
		switch (ConnectionState)
		{
		case SCS_NotConnected:
			state = EBattleshipSocketConnectionState::BPSocket_NotConnected;
			break;
		case SCS_Connected:
			state = EBattleshipSocketConnectionState::BPSocket_Connected;
			break;
		default:
			state = EBattleshipSocketConnectionState::BPSocket_ConnectionError;
			break;
		}
		SocketConnectionStateChanged.Broadcast(state);
	}
	uint32 PendingDataCount;
	if (Socket->HasPendingData(PendingDataCount))
	{
		if (PendingDataCount > 128) // Max 128 bytes per tick
		{
			PendingDataCount = 128;
		}
		TArray<uint8> Data;
		Data.Reserve(PendingDataCount);
		int32 DataRead;
		Socket->Recv(Data.GetData(), PendingDataCount, DataRead);
		Data.SetNum(DataRead);
		for (auto It = Data.CreateConstIterator(); It; ++It)
		{
			HandleByte({.Uns = *It });
		}
	}
}

bool UBattleShipSocketWrapper::Connect(const FString& IP, int32 Port)
{
	if (LastConnectionState == SCS_Connected)
	{
		UE_LOG(LogBattleship, Error, TEXT("Socket already connected"));
		return false;
	}
	const TSharedPtr<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Valid;
	Address->SetIp(*IP, Valid);
	if (!Valid)
	{
		return false;
	}
	Address->SetPort(Port);
	const auto Result = Socket->Connect(*Address);
	if (Result)
	{
		const auto* World = GetWorld();
		if (IsValid(World))
		{
			auto& TM = World->GetTimerManager();
			if (Timer.IsValid())
			{
				TM.ClearTimer(Timer);
			}
			TM.SetTimer(Timer, this, &UBattleShipSocketWrapper::Tick, 0.2f, true, 0.0);
		}
	}
	return Result;
}


void UBattleShipSocketWrapper::BeginDestroy()
{
	if (Socket)
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
	Super::BeginDestroy();
}
