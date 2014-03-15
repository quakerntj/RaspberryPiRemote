#pragma once

#include "remotecontrol/RemotePackage.h"
#include "remotecontrol/RemoteSocket.h"

class RemoteClient : public RemoteSocket {
public:
	RemoteClient();
	~RemoteClient();

	bool sendOpenDoorEvent(const char key[256]);
};
