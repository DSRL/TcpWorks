// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "tcpClient.generated.h"


UCLASS(BlueprintType, Blueprintable)
class TCPWORKS_API UtcpClient : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	UFUNCTION(BlueprintPure, Category = Sockets)
		static UtcpClient* ConstructUDPWrapper(const FString& Description, const FString& SenderSocketName, const FString& TheIP, const int32 ThePort, const int32 BufferSize,
			const bool AllowBroadcast, const bool Bound, const bool Reusable, const bool Blocking);

	static UtcpClient* Constructor();
	// bool dataReady;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Message", CompactNodeTitle = "Send", Keywords = "Send Message"), Category = "Networking")
		bool SendMessage(FString Message);
	UFUNCTION(BlueprintCallable, Category = "Networking")
		bool anyMessages();
	UFUNCTION(BluePrintCallable, Category = "Networking")
		FString getMessage();

	//FString StringFromBinaryArray(const TArray<uint8> BinaryArray);
	static FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	//static bool dataReady;
private:

	// Holds the socket we are sending on
	FSocket* SenderSocket;

	// Description for debugging
	FString SocketDescription;

	// Remote Address
	FIPv4Endpoint RemoteEndPoint;
	FIPv4Address RemoteAdress;

	// Socket Subsystem
	ISocketSubsystem* SocketSubsystem;
};