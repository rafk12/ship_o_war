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
	LastConnectionState = EBattleshipSocketConnectionState::BPSocket_NotConnected;
}

void UBattleShipSocketWrapper::HandleByte(FByteData Data)
{
	switch (CurrentState)
	{
	case EReadingState::None:
		{
			CurrentState = static_cast<EReadingState>(Data.Uns);
			CurrentStateData.Reset();
			return;
		}
	case EReadingState::Grid:
		{
			if (CurrentStateData.Num() == 0)
			{
				CurrentStateData.Add(Data.Sig);
				return;
			}
			BSCreateGridEvent.Broadcast(CurrentStateData[0], Data.Sig);
		}
		break;
	case EReadingState::Skin:
		BSSelectSkinEvent.Broadcast(Data.Sig);
		break;
	case EReadingState::Slot:
		{
			if (CurrentStateData.Num() < 4)
			{
				CurrentStateData.Add(Data.Sig);
				return;
			}
			BSRevealSlotEvent.Broadcast(CurrentStateData[0], CurrentStateData[1], CurrentStateData[2],
			                            CurrentStateData[3], static_cast<EBattleshipMapSlotState>(Data.Sig));
		}
		break;
	case EReadingState::Win:
		BSWinEvent.Broadcast(!!Data.Sig);
		break;
	case EReadingState::BoatCount:
		BSBoatCountEvent.Broadcast(Data.Sig);
		break;
	case EReadingState::Turn:
		BSTurnEvent.Broadcast(!!Data.Sig);
		break;
	}
	CurrentState = EReadingState::None;
}

void UBattleShipSocketWrapper::Tick()
{
	const auto ConnectionState = static_cast<EBattleshipSocketConnectionState>(Socket->GetConnectionState());
	if (ConnectionState != LastConnectionState)
	{
		LastConnectionState = ConnectionState;
		SocketConnectionStateChanged.Broadcast(ConnectionState);
	}
	uint32 PendingDataCount;
	if (Socket->HasPendingData(PendingDataCount))
	{
		if (PendingDataCount > 128) // Max 128 bytes per tick
		{
			PendingDataCount = 128;
		}
		static TArray<uint8> Data;
		Data.SetNum(PendingDataCount, false);
		int32 DataRead;
		Socket->Recv(Data.GetData(), PendingDataCount, DataRead);
		Data.SetNum(DataRead);
		for (auto It = Data.CreateConstIterator(); It; ++It)
		{
			FByteData ByteData;
			ByteData.Uns = *It;
			HandleByte(ByteData);
		}
	}
}


bool UBattleShipSocketWrapper::SocketConnectionGuarantee() const
{
	const auto Ok = Socket != nullptr && Socket->GetConnectionState() == SCS_Connected;
	if(!Ok)
	{
		UE_LOG(LogBattleship, Error, TEXT("Trying to use a disconnected socket"));
	}
	return Ok;
}

bool UBattleShipSocketWrapper::Connect(const FString& IP, int32 Port)
{
	if (Socket == nullptr)
	{
		UE_LOG(LogBattleship, Error, TEXT("Invalid socket (null)"));
		return false;
	}
	if (LastConnectionState == EBattleshipSocketConnectionState::BPSocket_Connected)
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

void UBattleShipSocketWrapper::Disconnect(const bool Recreate)
{
	const auto* World = GetWorld();
	if (IsValid(World))
	{
		auto& TM = World->GetTimerManager();
		if (Timer.IsValid())
		{
			TM.ClearTimer(Timer);
		}
	}
	Socket->Close();
	Socket = nullptr;
	if (Recreate)
	{
		Init();
	}
}

void UBattleShipSocketWrapper::SendSkin(const int32 Skin)
{
	if(!SocketConnectionGuarantee())
	{
		return;
	}
	uint8 Data[2];
	Data[0] = static_cast<uint8>(EWriteValues::Skin);
	Data[1] = static_cast<uint8>(Skin);
	int32 Sent;
	Socket->Send(Data, 2, Sent);
}

void UBattleShipSocketWrapper::SendClick(const int32 X, const int32 Y)
{
	if(!SocketConnectionGuarantee())
	{
		return;
	}
	uint8 Data[3];
	Data[0] = static_cast<uint8>(EWriteValues::Click);
	Data[1] = static_cast<uint8>(X);
	Data[2] = static_cast<uint8>(Y);
	int32 Sent;
	Socket->Send(Data, 3, Sent);
}


void UBattleShipSocketWrapper::BeginDestroy()
{
	if (Socket)
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
	Super::BeginDestroy();
}
