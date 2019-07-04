// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketWrapper.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"

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
	Socket->HasPendingData(PendingDataCount);
}

bool UBattleShipSocketWrapper::Connect(const FString& IP, int32 Port) const
{
	const TSharedPtr<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Valid;
	Address->SetIp(*IP, Valid);
	if (!Valid)
	{
		return false;
	}
	Address->SetPort(Port);
	return Socket->Connect(*Address);
}


void UBattleShipSocketWrapper::BeginDestroy()
{
	Super::BeginDestroy();
	if (Socket)
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}
