#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "remotecontrol/RemoteSocket.h"

RemoteSocket::RemoteSocket(bool startAsServer) :
        mIsServer(startAsServer),
        mPort(0),
        mSocket(-1),
        mCli(-1),
        mIsConnected(false),
        mpListener(NULL) {
    strncpy(mpServerIP, "127.0.0.1", 16);
}

RemoteSocket::~RemoteSocket() {
    if (mIsConnected)
        disconnect();
}

void RemoteSocket::setServerIP(const char *ip) {
    strncpy(mpServerIP, ip, 16);
}

void RemoteSocket::setPort(int port) {
    mPort = port;
}

void RemoteSocket::setListener(RemoteDataListener *listener) {
    mpListener = listener;
}

bool RemoteSocket::connect() {
    if (mIsConnected)
        return false;

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(mPort);
    dest.sin_addr.s_addr = mIsServer ? INADDR_ANY : inet_addr(mpServerIP);

    printf("create socket...\n");
    if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket create failed! %s\n", strerror(errno));
        return false;
    }

    if (mIsServer) {
        printf("binding...\n");
        if (bind(mSocket, (struct sockaddr*) &dest, sizeof(dest)) < 0) {
            fprintf(stderr, "Unable to bind socket!\n");
            return false;
        }

        printf("listening...\n");
        if (::listen(mSocket, 1) < 0) {
            fprintf(stderr, "Unable to listen on socket!\n");
            return false;
        }
    } else {
        printf("connect to server...\n");
        if(::connect(mSocket, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
            fprintf(stderr, "socket connect failed! %s\n", strerror(errno));
            return false;
        }
        printf("connected\n");

        if (mpListener)
            mpListener->onConnected();
    }

    return mIsConnected = true;
}

void RemoteSocket::disconnect() {
    if (!mIsConnected)
        return;

    printf("connection disconnect\n");

    int socket = mSocket;
    mSocket = -1;
    shutdown(socket, SHUT_RDWR);
    ::close(socket);
    mIsConnected = false;
}

bool RemoteSocket::isConnected() {
    return mIsConnected;
}

bool RemoteSocket::send(const struct RemotePackage *p) {
    if (!p || !p->len)
        return false;

    int target = mIsServer ? mCli : mSocket;
    if (target < 0) {
        fprintf(stderr, "connection is not established\n");
        return false;
    }

    int ret = ::send(target, p, p->len, 0);
    if (ret < 0) {
        fprintf(stderr, "ret = %d, %s\n", ret, strerror(errno));

        if (!mIsServer)
            disconnect();

        return false;
    }

    return true;
}

void RemoteSocket::receive() {
    if (!mIsConnected) {
	    fprintf(stderr, "No Connection\n");
        return;
    }

    RemotePackage * pkg;
    char buffer[sizeof(RemoteOpenDoorEvent) * 2];

    while (mSocket >= 0) {
        int count;

        if (mIsServer) {
            struct sockaddr cliaddr;
            socklen_t alen;

            do {
                printf("wait for acception.\n");
                alen = sizeof(cliaddr);
                mCli = accept(mSocket, &cliaddr, &alen);
                printf("Accepted\n");
            } while (mCli < 0  && errno == EINTR);
            printf("client connected\n");

            if (mpListener)
                mpListener->onConnected();
        }

        printf("receiving data...\n");
        while (1) {
            count = recv(mIsServer ? mCli : mSocket, buffer, sizeof(buffer), 0);
            if (count <= 0) {
                mCli = -1;
                break;
            }

            while (count > 0) {
                if (count >= static_cast<int>(sizeof(RemotePackage))) {
                    pkg = reinterpret_cast<RemotePackage*>(buffer);
                    if (count >= pkg->len) {
                        if (mpListener)
                            mpListener->onData(pkg);
                        count -= pkg->len;
                    }
                } else {
                    count = 0;
                }
            }
        }
    }

    printf("leave receive()\n");
}
