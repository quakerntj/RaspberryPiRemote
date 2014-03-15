all: doorserver doorclient

doorserver: RemoteSocket.cpp RemoteServer.cpp RemoteClient.cpp server.cpp
	g++ $^ -o $@ -g -Iinclude

doorclient: RemoteSocket.cpp RemoteServer.cpp RemoteClient.cpp client.cpp
	g++ $^ -o $@ -g -Iinclude

clean:
	-rm doorserver
	-rm doorclient
