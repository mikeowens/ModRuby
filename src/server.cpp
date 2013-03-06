#define EXPORT_MODCPP

#include "server.h"

using namespace apache;

Server::Server(const server_rec *s)
{
    _sr = s;
    _next = s->next ? new Server(s->next) : NULL;
}

void Server::operator=(const server_rec* sr)
{
    _sr = sr;
    _next = sr->next ? new Server(sr->next) : NULL;
}

Server::~Server()
{
    _sr = NULL;

    delete _next;
}

void* Server::get_module_config(module *m)
{
    return ap_get_module_config(_sr->module_config, m);
}
