#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include "remotecontrol/RemotePackage.h"
#include "remotecontrol/RemoteClient.h"

//#define handle_error_en(en, msg) \
//    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

//#define handle_error(msg) \
//    do { perror(msg); exit(EXIT_FAILURE); } while (0)

//static void * thread_start(void *arg) {
//    struct thread_info *tinfo = (struct thread_info *) arg;
//    char *uargv, *p;

//    printf("Thread %d: top of stack near %p; argv_string=%s\n",
//        tinfo->thread_num, &p, tinfo->argv_string);

//    uargv = strdup(tinfo->argv_string);
//    if (uargv == NULL)
//        handle_error("strdup");

class Listener : public RemoteDataListener {
    const char * mKey;

public:
    Listener(const char* key) : mKey(key) {}
    ~Listener() {}
    void onData(RemotePackage const * pkg) {
        if (pkg->type == REMOTE_EVENT_OPEN_DOOR) {
            RemoteOpenDoorEvent const * event = reinterpret_cast<RemoteOpenDoorEvent const*>(pkg);
            for (int i = 0; i < 256; i++) {
                if (event->key[i] != mKey[i])
                    goto wrongKey;
            }
            // open door;
        }
        // avoid attack.
        sleep(2);
        return;
    wrongKey:
        // should send error status back to client.
        ;
    }
    void onConnected() {
    }
};

int main(int argc, char** argv) {
    const char * ip = "127.0.0.1";
    int port = 8768;
    char key[256] = "";
    const char * doorkey = "doorkey.txt";
    Listener * listener;
    RemoteClient * client;

    char * str;
    char * ptr = argv[0];
    for (int i = 1; i < argc; i++) {
        ptr = argv[i];
        if (!strcmp(ptr, "-p")){
            if (++i < argc) {
                continue;
                int port = atoi(argv[i]);
                if (port < 5000) {
                    fprintf(stderr, "Port number need greater than 5000\n");
                    return -8;
                }
            } else {
                fprintf(stderr, "Need port number\n");
                return -8;
            }
        }
        if (!strcmp(ptr, "-i")) {
            if (++i < argc) {
                ip = argv[i];
                continue;
            } else {
                return -8;
            }
        }
        if (!strcmp(ptr, "-k")) {
            if (++i < argc) {
                char * keyFile = argv[i];
                int fd = open(keyFile, O_RDONLY);
                ssize_t len;
                if (fd < 0) {
                    fprintf(stderr, "Fail to open KeyFile\n");
                    return -8;
                }
                len = read(fd, key, 256);
                if (len != 256) {
                    fprintf(stderr, "KeyFile didn't have key of 256 charactor.\n");
                    return -8;
                }
                close(fd);
                continue;
            } else {
                fprintf(stderr, "Need input KeyFile\n");
                return -8;
            }
        }
        if (!strcmp(ptr, "-h")) {
            char * cmd = basename(argv[0]);
            printf("%s [-k <keyfile>][-p <port>]\n"
                "    keyfile:  file with 256 key charactor.  Default file is %s.\n"
                "    port:  port to listen.  Default port is %d.\n",
                cmd, doorkey, port);
            return 0;
        }
        fprintf(stderr, "Unknown command\n");
        return -8;
    }
    if (key[0] == 0) {
        int fd = open(doorkey, O_RDONLY);
        ssize_t len;
        if (fd < 0) {
            fprintf(stderr, "Fail to open %s\n", doorkey);
            return -8;
        }
        len = read(fd, key, 256);
        if (len != 256) {
            fprintf(stderr, "%s didn't have key of 256 charactor.\n", doorkey);
            return -8;
        }
        close(fd);
    }

    printf("Starting client %s:%d\n", ip, port);

    //listener = new Listener(key);
    client = new RemoteClient();
    client->setServerIP(ip);
    client->setPort(port);
    //client->setListener(listener);

    // start block
    client->connect();
    client->sendOpenDoorEvent(key);
    client->disconnect();
    delete client;
    return 0;
}
