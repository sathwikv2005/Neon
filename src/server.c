#include "server.h"

Server server;

void initServer() { initTable(&server.strings); }

void freeServer() { freeTable(&server.strings); }