// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketWrapper.h"
#include "SocketSubsystem.h"

USocketWrapper::USocketWrapper()
{
}

void USocketWrapper::Initialize(const FString& name, bool udp)
{
	socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, name, udp);
}

void USocketWrapper::BeginDestroy()
{
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(socket);
}
