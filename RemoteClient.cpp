#include <stdio.h>
#include <string.h>
#include "remotecontrol/RemoteClient.h"

RemoteClient::RemoteClient() : RemoteSocket(false) {
}

RemoteClient::~RemoteClient() {
}

bool RemoteClient::sendOpenDoorEvent(const char key[256]) {
	if (!mIsConnected) {
	    fprintf(stderr, "No Connection\n");
		return false;
	}
	RemoteOpenDoorEvent event;
	memcpy(event.key, key, 256);
//    for (int i = 0; i < 256; i++) {
//        putchar(event.key[i]);
//    }
	event.package.len = sizeof(struct RemoteOpenDoorEvent);
	event.package.type = REMOTE_EVENT_OPEN_DOOR;
	return send(&(event.package));
}
