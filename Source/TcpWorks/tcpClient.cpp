// Fill out your copyright notice in the Description page of Project Settings.

#include "tcpClient.h"
#include <string>
UtcpClient* UtcpClient::Constructor()
{
	return (UtcpClient*)StaticConstructObject(UtcpClient::StaticClass());
}
UtcpClient* UtcpClient::ConstructUDPWrapper(const FString& Description, const FString& SenderSocketName, const FString& TheIP, const int32 ThePort, const int32 BufferSize,
	const bool AllowBroadcast, const bool Bound, const bool Reusable, const bool Blocking)
{

	UtcpClient* wrapper = Constructor();

	wrapper->SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address::Parse(TheIP, wrapper->RemoteAdress);
	addr->SetIp(wrapper->RemoteAdress.Value);
	addr->SetPort(ThePort);

	wrapper->RemoteEndPoint = FIPv4Endpoint(wrapper->RemoteAdress, ThePort);
	// First set our socket null
	wrapper->SenderSocket = nullptr;
	if (wrapper->SocketSubsystem != nullptr) // If socket subsytem is good
	{
		wrapper->SenderSocket = wrapper->SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TCP_TEST"), false);
		bool connected = wrapper->SenderSocket->Connect(*addr);

		if (connected) // Is our socket created
		{
			// Setup the socket 
			/*bool Error = !wrapper->SenderSocket->SetNonBlocking(!Blocking) ||
				!wrapper->SenderSocket->SetReuseAddr(Reusable) ||
				!wrapper->SenderSocket->SetBroadcast(AllowBroadcast) ||
				!wrapper->SenderSocket->SetRecvErr();

			if (!Error)
			{
				if (Bound)
				{
					Error = !wrapper->SenderSocket->Bind(*wrapper->RemoteEndPoint.ToInternetAddr());
				}
			}

			if (!Error)
			{
				int32 OutNewSize;

				wrapper->SenderSocket->SetReceiveBufferSize(BufferSize, OutNewSize);
				wrapper->SenderSocket->SetSendBufferSize(BufferSize, OutNewSize);
			}*/
		}
		else
		{
			wrapper->SocketSubsystem->DestroySocket(wrapper->SenderSocket);
			wrapper->SenderSocket = nullptr;
		}


	}
	return wrapper;
}
bool UtcpClient::SendMessage(FString Message)
{
	if (!SenderSocket) return false;
	int32 BytesSent;
	FTimespan waitTime = FTimespan(10);

	TCHAR *serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;

	bool success = SenderSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndPoint.ToInternetAddr());
	return success;
	/*
	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP_TEST"), false);

	FString address = TEXT("127.0.0.1");

	FIPv4Address ip;
	FIPv4Address::Parse(address, ip);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(2021);

	bool connected = Socket->Connect(*addr);

	if (connected)
	{
		FString serialized = Message;
		TCHAR *serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;

		bool successful = wrapper->SenderSocket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (successful)
		{
			//Binary Array!
			TArray<uint8> ReceivedData;
			_sleep(10);
			uint32 Size;
			while (Socket->HasPendingData(Size))
			{
				ReceivedData.Init(0,FMath::Min(Size, 65507u));

				int32 Read = 0;
				Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			}
			//~~
			Socket->Close();
			const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
			return ReceivedUE4String;
		}
		Socket->Close();
		return "-1";
	}
	Socket->Close();
	return "-1";*/
}

bool UtcpClient::anyMessages()
{
	if (!SenderSocket) return false;
	uint32 Size;

	if (SenderSocket->HasPendingData(Size))
	{
		return true;
	}

	return false;
}
FString UtcpClient::getMessage()
{
	if (!SenderSocket) return "-1";
	TArray<uint8> ReceivedData;
	uint32 Size;
	while (SenderSocket->HasPendingData(Size))
	{
		ReceivedData.Init(0, FMath::Min(Size, 65507u));

		int32 Read = 0;
		SenderSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	return ReceivedUE4String;
}
//Rama's String From Binary Array
/*
FString UtcpClient::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	BinaryArray.Add(0); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
						// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
						// Use UTF8_TO_TCHAR if needed.
						// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
						// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData()));
}*/


FString UtcpClient::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());

	return FString(cstr.c_str());
}


