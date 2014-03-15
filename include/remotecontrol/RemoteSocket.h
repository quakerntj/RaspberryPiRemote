#pragma once

#include <remotecontrol/RemotePackage.h>

class RemoteDataListener {
public:
    RemoteDataListener() {}
    virtual ~RemoteDataListener() {}
    virtual void onData(RemotePackage const *) = 0;
    virtual void onConnected() {}
};

class RemoteSocket {
public:
    RemoteSocket(bool startAsServer);
    ~RemoteSocket();

    void setServerIP(const char *ip);
    void setPort(int port);
    void setListener(RemoteDataListener *listener);
    bool connect();
    void disconnect();
    bool isConnected();
    bool send(const struct RemotePackage *p);
    void receive();

protected:
    bool mIsServer;
    char mpServerIP[16];
    int mPort;
    int mSocket;
    int mCli;
    bool mIsConnected;
    RemoteDataListener *mpListener;
};
