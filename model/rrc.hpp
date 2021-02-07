#pragma once
#include <router.hpp>
#include <node.hpp>
#include <state.hpp>
#include <list>
#include <opnet.h>
#define OPNET
#include <balloc.hpp>

#define ROUTE_UPDATE_INTERVAL 5

namespace rrc_ns
{
typedef enum
{
    SRB_DL,
    SRB_UL,
    NDCH_DL, //Neighbor discovery channel
    NDCH_UL,
    LCCH_DL, // Link control channel
    LCCH_UL,
    LBCH_DL, // Link broadcast channel
    LBCH_UL,
    RCCH_DL, //
    RCCH_UL,
    BACH_DL, // Beacon allocation channel
    BACH_UL,
    BCCH_DL, // Baacon allocation control
    BCCH_UL,
    LUCH_DL, //Link info unicast channel
    LUCH_UL,
    RELEASE_UL
} strm_t;

typedef enum
{
    ROUTE_UPDATE_TIMEOUT
} event_t;
typedef enum
{
    ON_ND1_REQ,
    ON_NDCH_UL,
    ON_LINK_UPDATE,
    ON_NETSTATE_UPDATE,
    ON_ENDSIM,
    ON_TIMEOUT,
    ON_BALLOC,
    ON_QUEUE_REPORT,
    ON_LINK_UNICAST,
    ON_RELEASE
} intrpt_t;

static const char *NAME_INTRPT[] = {"ON_ND1_REQ", "ON_NDCH_UL", "ON_LINK_UPDATE", "ON_NETSTATE_UPDATE", "ON_ENDSIM", "ON_TIMEOUT"};

typedef class rrc
{
private:
    int id;
    Objid obj;
    router<node, edge<node>> router;
    unsigned long long qcnt;
    unsigned long long Q[48];
    balloc::allocator *allocator;
    bool isstar;

public:
    rrc();
    ~rrc();

    void print_forward_table();
    void print_beacon_table();
    void print_neighbors();
    void alloc_ext_beacon(int id, int number);
    void proc_ext_beacon_req(Packet *p);
    void proc_nd(const balloc::balloc_msg_t &msg);
    void proc_link_update(Packet *p);
    void proc_balloc(Packet *p);
    void update_beacon_table();
    void update_forward_table();
    void send_link_broadcast();
    void send_link_unicast(int dst);
    int calc_potential_free(int qid);
    Packet *generate_link_broadcast();
    friend class idle;

} rrc_t;

typedef class idle : state
{
public:
    idle();
    ~idle();
    idle(rrc_t *p);
    void enter();
    void exit();
    void proc(int cond);
    bool cond(int cond);

private:
    rrc_t *parent;
} idle_t;

void msg2nd(const balloc::balloc_msg_t &msg,Packet *p);
balloc::balloc_msg_t nd2msg(Packet *p);

Packet *msg2pkt(const balloc::balloc_msg_t &msg);
balloc::balloc_msg_t pkt2msg(Packet *p);

}; // namespace rrc_ns