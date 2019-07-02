// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketInteropLibrary.h"

USocketWrapper* USocketInteropLibrary::CreateSocket(UObject *outer, const FString &name, bool udp) {
	USocketWrapper* Obj = NewObject<USocketWrapper>(outer);
	Obj->Initialize(name, udp);
	return Obj;
} 
