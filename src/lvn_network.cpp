#include "levikno.h"
#include "levikno_internal.h"

#include "enet/enet.h"

namespace lvn
{

LvnResult initNetworkingContext()
{
    if (enet_initialize() != 0)
    {
        LVN_CORE_ERROR("failed to initialize networking context");
        return Lvn_Result_Failure;
    }

    LVN_CORE_TRACE("networking context initialized");
    return Lvn_Result_Success;
}

void terminateNetworkingContext()
{
    enet_deinitialize();
    LVN_CORE_TRACE("networking context terminated");
}

LvnResult createSocket(LvnSocket** socket, const LvnSocketCreateInfo* createInfo)
{
    LvnContext* lvnctx = lvn::getContext();

    *socket = lvn::createObject<LvnSocket>(lvnctx, Lvn_Stype_Socket);
    LvnSocket* socketPtr = *socket;

    ENetAddress address;
    address.host = createInfo->address.host;
    address.port = createInfo->address.port;

    if (createInfo->type == Lvn_SocketType_Client)
    {
        socketPtr->socket = enet_host_create(nullptr, createInfo->connectionCount, createInfo->channelCount, createInfo->inBandWidth, createInfo->outBandWidth);
    }
    else if (createInfo->type == Lvn_SocketType_Server)
    {
        socketPtr->socket = enet_host_create(&address, createInfo->connectionCount, createInfo->channelCount, createInfo->inBandWidth, createInfo->outBandWidth);
    }

    if (socketPtr->socket == nullptr)
    {
        LVN_CORE_ERROR("createSocket(LvnSocket**, LvnSocketCreateInfo*) | an error occured while trying to create socket");
        return Lvn_Result_Failure;
    }

    socketPtr->connection = nullptr;
    socketPtr->type = createInfo->type;
    socketPtr->address = createInfo->address;
    socketPtr->connectionCount = createInfo->connectionCount;
    socketPtr->channelCount = createInfo->channelCount;
    socketPtr->inBandWidth = createInfo->inBandWidth;
    socketPtr->outBandWidth = createInfo->outBandWidth;

    LVN_CORE_TRACE("created socket: (%p), address: (%u:%u)", socket, createInfo->address.host, createInfo->address.port);
    return Lvn_Result_Success;
}

void destroySocket(LvnSocket* socket)
{
    if (socket == nullptr) { return; }
    LvnContext* lvnctx = lvn::getContext();
    enet_host_destroy(static_cast<ENetHost*>(socket->socket));
    lvn::destroyObject(lvnctx, socket, Lvn_Stype_Socket);
}

LvnSocketCreateInfo configSocketClientInit(uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth)
{
    LvnSocketCreateInfo createInfo{};
    createInfo.type = Lvn_SocketType_Client;
    createInfo.connectionCount = connectionCount;
    createInfo.channelCount = channelCount;
    createInfo.inBandWidth = inBandwidth;
    createInfo.outBandWidth = outBandWidth;

    return createInfo;
}

LvnSocketCreateInfo configSocketServerInit(LvnAddress address, uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth)
{
    LvnSocketCreateInfo createInfo{};
    createInfo.type = Lvn_SocketType_Server;
    createInfo.address = address;
    createInfo.connectionCount = connectionCount;
    createInfo.channelCount = channelCount;
    createInfo.inBandWidth = inBandwidth;
    createInfo.outBandWidth = outBandWidth;

    return createInfo;
}

uint32_t socketGetHostFromStr(const char* host)
{
    ENetAddress address{};
    enet_address_set_host(&address, host);
    return address.host;
}

LvnResult socketConnect(LvnSocket* socket, LvnAddress* address, uint32_t channelCount, uint32_t milliseconds)
{
    if (socket->type != Lvn_SocketType_Client)
    {
        LVN_CORE_ERROR("cannot use socket (%p) with type that is not client to connect", socket->socket);
        return Lvn_Result_Failure;
    }

    ENetAddress enetAddress;
    enetAddress.host = address->host;
    enetAddress.port = address->port;

    socket->address.host = address->host;
    socket->address.port = address->port;
    socket->connection = enet_host_connect(static_cast<ENetHost*>(socket->socket), &enetAddress, channelCount, 0);

    if (socket->connection == nullptr)
    {
        LVN_CORE_ERROR("no available peers for initiating a connection on socket (%p)", socket);
        return Lvn_Result_Failure;
    }

    ENetEvent event;
    if (enet_host_service(static_cast<ENetHost*>(socket->socket), &event, milliseconds) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        return Lvn_Result_Success;
    }

    enet_peer_reset(static_cast<ENetPeer*>(socket->connection));
    return Lvn_Result_TimeOut;
}

LvnResult socketDisconnect(LvnSocket* socket, uint32_t milliseconds)
{
    if (socket->type != Lvn_SocketType_Client)
    {
        LVN_CORE_ERROR("cannot use socket (%p) with type that is not client to disconnect", socket->socket);
        return Lvn_Result_Failure;
    }
    enet_peer_disconnect(static_cast<ENetPeer*>(socket->connection), 0);

    ENetEvent event;
    if (enet_host_service(static_cast<ENetHost*>(socket->socket), &event, milliseconds) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                return Lvn_Result_Success;
            }

            default:
            {
                LVN_CORE_WARN("unknown disconnect event received on socket (%p)", socket);
                break;
            }
        }
    }

    enet_peer_reset(static_cast<ENetPeer*>(socket->connection));
    return Lvn_Result_Success;
}

void socketSend(LvnSocket* socket, uint8_t channel, LvnPacket* packet)
{
    LVN_CORE_ASSERT(packet != nullptr, "packet is nullptr when trying to send packet through socket");

    ENetPacket* enetPacket = enet_packet_create(packet->data.data(), packet->data.memsize(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(static_cast<ENetPeer*>(socket->connection), channel, enetPacket);
    enet_host_flush(static_cast<ENetHost*>(socket->socket));
}

LvnResult socketReceive(LvnSocket* socket, LvnPacket* packet, uint32_t milliseconds)
{
    LVN_CORE_ASSERT(packet != nullptr, "packet is nullptr when trying to receive packet from socket");

    ENetEvent event;
    if (enet_host_service(static_cast<ENetHost*>(socket->socket), &event, milliseconds) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
    {
        packet->data = LvnBin(event.packet->data, event.packet->dataLength);
        enet_packet_destroy(event.packet);
        return Lvn_Result_Success;
    }

    return Lvn_Result_TimeOut;
}

} /* namespace lvn */
