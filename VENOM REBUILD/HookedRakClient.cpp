#include "plugin.h"

bool HookedRakClientInterface::RPC(int* uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (uniqueID == nullptr)
		return Dark::SAMP::getRakNet()->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
	if (!OnRPC(*uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp))
		return false;

	return Dark::SAMP::getRakNet()->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}

bool HookedRakClientInterface::Send(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (OnPacket(bitStream, priority, reliability, orderingChannel) == false) return true;
	return Dark::SAMP::getRakNet()->Send(bitStream, priority, reliability, orderingChannel);
}

Packet *HookedRakClientInterface::Receive(void)
{
	Packet *packet = Dark::SAMP::getRakNet()->Receive();
	
	if (packet != nullptr)
	{
		//receive packets here
	}

	return packet;
}

bool HookedRakClientInterface::Connect(const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer)
{
	return Dark::SAMP::getRakNet()->Connect(host, serverPort, clientPort, depreciated, threadSleepTimer);
}

void HookedRakClientInterface::Disconnect(unsigned int blockDuration, unsigned char orderingChannel)
{
	Dark::SAMP::getRakNet()->Disconnect(blockDuration, orderingChannel);
}

void HookedRakClientInterface::InitializeSecurity(const char *privKeyP, const char *privKeyQ)
{
	Dark::SAMP::getRakNet()->InitializeSecurity(privKeyP, privKeyQ);
}

void HookedRakClientInterface::SetPassword(const char *_password)
{
	Dark::SAMP::getRakNet()->SetPassword(_password);
}

bool HookedRakClientInterface::HasPassword(void) const
{
	return Dark::SAMP::getRakNet()->HasPassword();
}

bool HookedRakClientInterface::Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	return Dark::SAMP::getRakNet()->Send(data, length, priority, reliability, orderingChannel);
}

void HookedRakClientInterface::DeallocatePacket(Packet *packet)
{
	Dark::SAMP::getRakNet()->DeallocatePacket(packet);
}

void HookedRakClientInterface::PingServer(void)
{
	Dark::SAMP::getRakNet()->PingServer();
}

void HookedRakClientInterface::PingServer(const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections)
{
	Dark::SAMP::getRakNet()->PingServer(host, serverPort, clientPort, onlyReplyOnAcceptingConnections);
}

int HookedRakClientInterface::GetAveragePing(void)
{
	return Dark::SAMP::getRakNet()->GetAveragePing();
}

int HookedRakClientInterface::GetLastPing(void) const
{
	return Dark::SAMP::getRakNet()->GetLastPing();
}

int HookedRakClientInterface::GetLowestPing(void) const
{
	return Dark::SAMP::getRakNet()->GetLowestPing();
}

int HookedRakClientInterface::GetPlayerPing(const PlayerID playerId)
{
	return Dark::SAMP::getRakNet()->GetPlayerPing(playerId);
}

void HookedRakClientInterface::StartOccasionalPing(void)
{
	Dark::SAMP::getRakNet()->StartOccasionalPing();
}

void HookedRakClientInterface::StopOccasionalPing(void)
{
	Dark::SAMP::getRakNet()->StopOccasionalPing();
}

bool HookedRakClientInterface::IsConnected(void) const
{
	return Dark::SAMP::getRakNet()->IsConnected();
}

unsigned int HookedRakClientInterface::GetSynchronizedRandomInteger(void) const
{
	return Dark::SAMP::getRakNet()->GetSynchronizedRandomInteger();
}

bool HookedRakClientInterface::GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer)
{
	return Dark::SAMP::getRakNet()->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool HookedRakClientInterface::DeleteCompressionLayer(bool inputLayer)
{
	return Dark::SAMP::getRakNet()->DeleteCompressionLayer(inputLayer);
}

void HookedRakClientInterface::RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms))
{
	Dark::SAMP::getRakNet()->RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
}

void HookedRakClientInterface::RegisterClassMemberRPC(int* uniqueID, void *functionPointer)
{
	Dark::SAMP::getRakNet()->RegisterClassMemberRPC(uniqueID, functionPointer);
}

void HookedRakClientInterface::UnregisterAsRemoteProcedureCall(int* uniqueID)
{
	Dark::SAMP::getRakNet()->UnregisterAsRemoteProcedureCall(uniqueID);
}

bool HookedRakClientInterface::RPC(int* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	return Dark::SAMP::getRakNet()->RPC(uniqueID, data, bitLength, priority, reliability, orderingChannel, shiftTimestamp);
}

void HookedRakClientInterface::SetTrackFrequencyTable(bool b)
{
	Dark::SAMP::getRakNet()->SetTrackFrequencyTable(b);
}

bool HookedRakClientInterface::GetSendFrequencyTable(unsigned int outputFrequencyTable[256])
{
	return Dark::SAMP::getRakNet()->GetSendFrequencyTable(outputFrequencyTable);
}

float HookedRakClientInterface::GetCompressionRatio(void) const
{
	return Dark::SAMP::getRakNet()->GetCompressionRatio();
}

float HookedRakClientInterface::GetDecompressionRatio(void) const
{
	return Dark::SAMP::getRakNet()->GetDecompressionRatio();
}

void HookedRakClientInterface::AttachPlugin(void *messageHandler)
{
	Dark::SAMP::getRakNet()->AttachPlugin(messageHandler);
}

void HookedRakClientInterface::DetachPlugin(void *messageHandler)
{
	Dark::SAMP::getRakNet()->DetachPlugin(messageHandler);
}

BitStream * HookedRakClientInterface::GetStaticServerData(void)
{
	return Dark::SAMP::getRakNet()->GetStaticServerData();
}

void HookedRakClientInterface::SetStaticServerData(const char *data, const int length)
{
	Dark::SAMP::getRakNet()->SetStaticServerData(data, length);
}

BitStream * HookedRakClientInterface::GetStaticClientData(const PlayerID playerId)
{
	return Dark::SAMP::getRakNet()->GetStaticClientData(playerId);
}

void HookedRakClientInterface::SetStaticClientData(const PlayerID playerId, const char *data, const int length)
{
	Dark::SAMP::getRakNet()->SetStaticClientData(playerId, data, length);
}

void HookedRakClientInterface::SendStaticClientDataToServer(void)
{
	Dark::SAMP::getRakNet()->SendStaticClientDataToServer();
}

PlayerID HookedRakClientInterface::GetServerID(void) const
{
	return Dark::SAMP::getRakNet()->GetServerID();
}

PlayerID HookedRakClientInterface::GetPlayerID(void) const
{
	return Dark::SAMP::getRakNet()->GetPlayerID();
}

PlayerID HookedRakClientInterface::GetInternalID(void) const
{
	return Dark::SAMP::getRakNet()->GetInternalID();
}

const char* HookedRakClientInterface::PlayerIDToDottedIP(const PlayerID playerId) const
{
	return Dark::SAMP::getRakNet()->PlayerIDToDottedIP(playerId);
}

void HookedRakClientInterface::PushBackPacket(Packet *packet, bool pushAtHead)
{
	Dark::SAMP::getRakNet()->PushBackPacket(packet, pushAtHead);
}

void HookedRakClientInterface::SetRouterInterface(void *routerInterface)
{
	Dark::SAMP::getRakNet()->SetRouterInterface(routerInterface);
}

void HookedRakClientInterface::RemoveRouterInterface(void *routerInterface)
{
	Dark::SAMP::getRakNet()->RemoveRouterInterface(routerInterface);
}

void HookedRakClientInterface::SetTimeoutTime(RakNetTime timeMS)
{
	Dark::SAMP::getRakNet()->SetTimeoutTime(timeMS);
}

bool HookedRakClientInterface::SetMTUSize(int size)
{
	return Dark::SAMP::getRakNet()->SetMTUSize(size);
}

int HookedRakClientInterface::GetMTUSize(void) const
{
	return Dark::SAMP::getRakNet()->GetMTUSize();
}

void HookedRakClientInterface::AllowConnectionResponseIPMigration(bool allow)
{
	Dark::SAMP::getRakNet()->AllowConnectionResponseIPMigration(allow);
}

void HookedRakClientInterface::AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength)
{
	Dark::SAMP::getRakNet()->AdvertiseSystem(host, remotePort, data, dataLength);
}

RakNetStatisticsStruct* const HookedRakClientInterface::GetStatistics(void)
{
	return Dark::SAMP::getRakNet()->GetStatistics();
}

void HookedRakClientInterface::ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance)
{
	Dark::SAMP::getRakNet()->ApplyNetworkSimulator(maxSendBPS, minExtraPing, extraPingVariance);
}

bool HookedRakClientInterface::IsNetworkSimulatorActive(void)
{
	return Dark::SAMP::getRakNet()->IsNetworkSimulatorActive();
}

PlayerIndex HookedRakClientInterface::GetPlayerIndex(void)
{
	return Dark::SAMP::getRakNet()->GetPlayerIndex();
}

bool HookedRakClientInterface::RPC_(int* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID)
{
	return Dark::SAMP::getRakNet()->RPC_(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID);
}