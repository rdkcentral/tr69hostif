// paramMgr.h
#ifndef PARAM_MGR_H
#define PARAM_MGR_H

#include <glib.h>

extern GHashTable* paramMgrhash;  // Declaration

typedef struct argsList {
    char logFileName[64];
    char confFile[100];
    int httpPort;
#ifndef NEW_HTTP_SERVER_DISABLE
    int httpServerPort; //new HTTP Server Port for JSON Requests
#endif
} T_ARGLIST;

#endif
