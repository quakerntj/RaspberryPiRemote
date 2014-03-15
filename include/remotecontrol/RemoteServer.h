#pragma once

#include "remotecontrol/RemotePackage.h"
#include "remotecontrol/RemoteSocket.h"

class RemoteServer : public RemoteSocket {
  public:
    RemoteServer();
    ~RemoteServer();
};
