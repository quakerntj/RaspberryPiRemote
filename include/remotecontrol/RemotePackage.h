#pragma once

struct RemotePackage {
	int len;
	int type;
};

struct RemoteOpenDoorEvent {
	struct RemotePackage package;
	char key[256];
};

enum {
	REMOTE_EVENT_OPEN_DOOR,
};

/* Example
	void onData(RemotePackage * pkg) {
		if (pkg->type == REMOTE_EVENT_OPEN_DOOR) {
			RemoteSensorEvent * event = PKG_TO_SENSOR(p);
			event->x
			event->y
			event->z...
		}
	}
*/

