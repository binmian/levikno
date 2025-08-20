#include <levikno/levikno.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("program needs to be run as either client or server, ex: './clientServer client'\n");
        return -1;
    }

    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvn::createContext(&lvnCreateInfo);

    if (strcmp(argv[1], "client") == 0)
    {
        LvnSocketCreateInfo clientCreateInfo = lvn::configSocketClientInit(1, 2, 0, 0);
        LvnSocket* client;
        lvn::createSocket(&client, &clientCreateInfo);

        LvnAddress address{};
        address.host = lvn::socketGetHostFromStr("127.0.0.1");
        address.port = 1234;

        if (lvn::socketConnect(client, &address, 2, 5000) == Lvn_Result_Success)
        {
            printf("connection succeeded\n");

            const char* data = "hello world :3";
            LvnPacket packet{};
            packet.data = LvnBin((uint8_t*)data, strlen(data) + 1);
            lvn::socketSend(client, 0, &packet);
        }
        else
        {
            printf("connection failed\n");
        }

    }

    else if (strcmp(argv[1], "server") == 0)
    {
        LvnAddress address{};
        address.host = 0;
        address.port = 1234;

        LvnSocketCreateInfo serverCreateInfo = lvn::configSocketServerInit(address, 32, 2, 0, 0);
        LvnSocket* server;
        lvn::createSocket(&server, &serverCreateInfo);

        LvnPacket packet{};
        while (true)
        {
            LvnResult result = lvn::socketReceive(server, &packet, 1000);
            if (result == Lvn_Result_Success)
            {
                printf("packet recieved | length: %zu bytes, data: %s\n", packet.data.memsize(), (char*)packet.data.data());
            }
        }

        lvn::destroySocket(server);
    }

    return 0;
}
