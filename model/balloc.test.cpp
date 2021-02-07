#include <balloc.hpp>
#include <iostream>

using namespace balloc;
int main(int argc, char *argv[])
{
    allocator a(1), b(2), c(3);
    balloc_msg_t hello = a.get_hello_msg();
    auto ret = b.req_beacon(hello.info, PRIMARY_BALLOC);
    if (!ret.second)
        return 0;

    auto bt = b.get_beacon_table();
    for (int i = 0; i < 124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 2, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    balloc_msg_t req = std::move(ret.first.find(1)->second);

    auto r = a.on_recv(req);
    if (!r.second)
        return 0;

    bt = a.get_beacon_table();
    for (int i = 0; i <124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 1, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    balloc_msg_t ack = std::move(*r.first);
    delete r.first;

    r = b.on_recv(ack);
    if (!ret.second)
        return 0;

    bt = b.get_beacon_table();
    for (int i = 0; i < 124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 2, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    hello = c.get_hello_msg();

    ret = b.req_beacon(hello.info, PRIMARY_BALLOC);
    if (!ret.second)
        return 0;

    bt = b.get_beacon_table();
    for (int i = 0; i < 124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 2, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    req = std::move(ret.first.find(3)->second);

    r = c.on_recv(req);
    if (!r.second)
        return 0;

    bt = c.get_beacon_table();
    for (int i = 0; i < 124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 3, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    ack = std::move(*r.first);
    delete r.first;

    r = b.on_recv(ack);
    if (!ret.second)
        return 0;

    bt = b.get_beacon_table();
    for (int i = 0; i < 124; i++)
    {
        printf("[%d][%d] {id = %d rti = %d type = %d}\n", 2, i, bt.links[i].id, bt.links[i].mode, bt.links[i].type);
    }

    return 0;
}