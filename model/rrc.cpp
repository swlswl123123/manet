#include <rrc.hpp>
#include <opnet.h>
#include <log.h>
#include <packet.hpp>
#include <deque>
#include <algorithm>
#include <set>
#include <map>
#include <string.h>

namespace rrc_ns
{

rrc::rrc() : qcnt(0), isstar(0)
{
    obj = op_id_parent(op_id_self());
    if (op_ima_obj_attr_exists(obj, "user id"))
    {
        op_ima_obj_attr_get(obj, "user id", &id);
    }
    else
    {
        LOGD("user id not exits!");
    }
    router.add_vertex(node(id));

    allocator = new balloc::allocator(id);
};

rrc::~rrc(){};

void rrc::print_forward_table()
{
    auto vtx = router.get_vertice();
    auto fwd = router.get_forward_table();

    router.Dijkstra(router.get_vertex(id).first);

    for (auto n = vtx.begin(); n != vtx.end(); ++n)
    {
        // LOG("[%d]-.-.->[%d]", id, n->get_id());
        for (int i = 0; i < 34; i++)
        {
            if (n->get_bov().links[i].id != 0)
            {
                // LOG("[%d]NEIGHBOR:[%d] %s [%d]",id,n->get_id(),n->rti[i]?"->":"<-",n->bov[i]);
            }
        }
    }

    std::set<int> reachable;
    for (auto n = fwd.begin(); n != fwd.end(); ++n)
    {
        reachable.insert(n->first.get_id());
        reachable.insert(n->second.get_id());
        // LOG("[%d]->[%d]->[%d]", id, n->second.get_id(), n->first.get_id());
    }

    LOG("[%d]NB = %d/%d", id, reachable.size(), vtx.size());
};

void rrc::print_beacon_table()
{

    auto self = router.get_vertex(id).first;

    for (int i = 0; i < 34; i++)
    {
        LOG("[%d]BEACON_TABLE[%d] id = %d RTI = %d MODE = %d", id, i, self.get_bov().links[i].id, self.get_bov().links[i].mode, self.get_bov().links[i].type);
    }
}

void rrc::print_neighbors()
{
}

void rrc::alloc_ext_beacon(int dst, int num)
{
    node_t self = router.get_vertex(id).first;
    auto n = router.get_vertex(dst);
    if (!n.second)
    {
        return;
    }
    node_t nb = n.first;

    {
        using namespace balloc;

        int free = rrc::calc_potential_free(dst);
        LOG("[%d]free =%d", id, free);
        if (num > 0)
        {
            num = num > free ? free : num;
        }

        auto ret = allocator->req_beacon(n.first.get_bov(), EXTENDED_BALLOC, op_sim_time() * 1e6, num);

        if (ret.second)
        {
            auto bt = allocator->get_beacon_table();
            router.add_vertex(node(bt, allocator::session::get_seq_id()));
            update_beacon_table();
            update_forward_table();
        }

        for (auto j = ret.first.begin(); j != ret.first.end(); ++j)
        {
            Packet *p = op_pk_create(0);
            Packet *q = msg2pkt(j->second);
            op_pk_fd_set_int32(q, 252, op_sim_time()*1e6, 32);
            op_pk_fd_set_int32(p, 0, j->first, 8);
            op_pk_fd_set_pkt(p, 1, q, op_pk_total_size_get(q));
            op_pk_send(p, BACH_DL);
        }
    }
}

void rrc::proc_ext_beacon_req(Packet *p)
{
}

idle::idle(){
    // op_intrpt_schedule_self(op_sim_time()+1,ROUTE_UPDATE_TIMEOUT);
};
idle::~idle(){};
idle::idle(rrc_t *p) : parent(p){};
void idle::enter()
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();

    if (type == OPC_INTRPT_BEGSIM)
    {
        op_intrpt_schedule_self(op_sim_time() + parent->id % ROUTE_UPDATE_INTERVAL, ROUTE_UPDATE_TIMEOUT);
    }
}
void idle::exit(){};
void idle::proc(int cond)
{

    LOGD("[%d]rrc:%s", parent->id, NAME_INTRPT[cond]);

    switch (cond)
    {

    case ON_ND1_REQ:
    {
        Packet *p = op_pk_create_fmt("724nd");
        balloc::balloc_msg_t msg = parent->allocator->get_hello_msg();
        msg2nd(msg, p);
        op_pk_total_size_set(p, 7500);
        op_pk_send(p, NDCH_DL);
    }
    break;
    case ON_NDCH_UL:
    {
        Packet *p = op_pk_get(NDCH_UL);
        balloc::balloc_msg_t msg = nd2msg(p);
        parent->proc_nd(msg);
    }
    break;
    case ON_LINK_UPDATE:
    {

        LOGD("[%d]ON_LINK_UPDATE", parent->id);
        parent->proc_link_update(op_pk_get(LBCH_UL));
    }
    break;
    case ON_ENDSIM:
    {
        parent->print_forward_table();
        parent->print_beacon_table();
        parent->router.Dijkstra(parent->router.get_vertex(parent->id).first);
    }
    break;
    case ON_TIMEOUT:
    {
        if(!parent->isstar)
            parent->send_link_broadcast();
        op_intrpt_schedule_self(op_sim_time() + ROUTE_UPDATE_INTERVAL, ROUTE_UPDATE_TIMEOUT);
    }
    break;
    case ON_BALLOC:
    {
        LOG("[%d]ON_BALLOC", parent->id);
        parent->proc_balloc(op_pk_get(BACH_UL));
    }
    break;
    case ON_RELEASE:
    {
        LOG("[%d]ON_RELEASE", id);
        Packet* p = op_pk_get(RELEASE_UL);
        int d;
        op_pk_fd_get_int32(p, 0, &d);
        parent->allocator->release(d);
        auto ret_self = parent->router.get_vertex(id);
        unsigned long long t = ret_self.first.last_update;
        parent->router.add_vertex(node(parent->allocator->get_beacon_table(), t));
        auto ret = parent->router.get_vertex(d);
        parent->router.remove_vertex(ret.first);
        op_pk_destroy(p);
        parent->update_beacon_table();
        parent->update_forward_table();
    }
    break;
    case ON_QUEUE_REPORT:
    {
        // LOG("[%d]ON_QUEUE_REPORT", parent->id);
        Packet *p = op_pk_get(BCCH_UL);

        auto self = parent->router.get_vertex(parent->id).first;
        for (int i = 0; i < 20; i++)
        {
            unsigned long long size = 0;
            op_pk_fd_get_int64(p, i, (long long *)&size);
            parent->Q[i] = size;
            auto n = parent->router.get_vertex(i + 1);
            if (n.second)
            {
                auto nb = n.first;
                int width = self || nb;
                if ((int)(ceil((double)(parent->Q[i])) / BIT_PER_UNIT) > width)
                {

                    LOG("[%d]->[%d]BALLOC NEED = %d", parent->id, i + 1, (int)(ceil((double)(parent->Q[i]) / BIT_PER_UNIT)) - width);
                    // to find which beacon table dst locate
                    parent->alloc_ext_beacon(nb.get_id(), (int)(ceil((double)(parent->Q[i]) / BIT_PER_UNIT)) - width);
                }
                else if ((int)(ceil((double)(parent->Q[i]) / BIT_PER_UNIT)) < width && width > 1)
                {
                    LOG("[%d]->[%d]BALLOC RELEASE = %d", parent->id, i + 1, (int)(ceil((double)(parent->Q[i]) / BIT_PER_UNIT)) - width);
                    parent->alloc_ext_beacon(nb.get_id(), (int)(ceil((double)(parent->Q[i]) / BIT_PER_UNIT)) - width);
                }
            }
        }

        parent->qcnt++;
    }
    default:
        break;
    }
};
bool idle::cond(int cond)
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();
    switch (cond)
    {
    case ON_ND1_REQ:
        return (type == OPC_INTRPT_ACCESS && strm == NDCH_DL);
    case ON_NDCH_UL:
        return (type == OPC_INTRPT_STRM && strm == NDCH_UL);
    case ON_LINK_UPDATE:
        return (type == OPC_INTRPT_STRM && strm == LBCH_UL);
    case ON_ENDSIM:
        return (type == OPC_INTRPT_ENDSIM);
    case ON_TIMEOUT:
        return (type == OPC_INTRPT_SELF && code == ROUTE_UPDATE_TIMEOUT);
    case ON_QUEUE_REPORT:
        return (type == OPC_INTRPT_STRM && strm == BCCH_UL);
    case ON_BALLOC:
        return (type == OPC_INTRPT_STRM && strm == BACH_UL);
    case ON_RELEASE:
        return (type == OPC_INTRPT_STRM && strm == RELEASE_UL);
    default:
        return false;
    }
};

int rrc::calc_potential_free(int qid)
{
    using namespace balloc;
    int ret = 0;
    int free = 0;
    long long sum[48] = {0};

    auto bt = allocator->get_beacon_table();

    for (int i = 0; i < 34; i++)
    {
        if (bt.links[i].type == VACCANT)
        {
            free++;
        }
    }
    LOG("[%d]BALLOC FREE = %d", id, free);

    for (int i = 0; i < 20; i++)
    {
        if (qid == i + 1)
            continue;
        sum[i] = Q[i];
        sum[i] = sum[i] / BIT_PER_UNIT;

        for (int j = 0; j < 34; j++)
        {
            if (bt.links[j].id == i + 1 && bt.links[j].mode == SEND)
            {
                sum[i]--;
            }
        }
        if (sum[i] < 0)
        {
            sum[i] = 0;
        }
        ret += sum[i];
        if (id == 2)
        {
            printf("ret::%d\n",ret);
        }
    }
    int self_need = Q[qid-1] / BIT_PER_UNIT;
    LOG("[%d]self need:%d ,ret:%d", id, self_need, ret);

    LOG("[%d]BALLOC POT FREE = %d", id, (int)ceil(free * ((double)self_need / (ret+self_need))));
    return (int)ceil(free * ((double)self_need / (ret+self_need)));
}

void rrc::proc_balloc(Packet *p)
{
    using namespace balloc;
    int time_stamp, cur_time = op_sim_time() * 1e6, type;
    op_pk_fd_get_int32(p, 2, &type);
    if (type == 1)
    {
        op_pk_fd_get_int32(p, 252, &time_stamp);
    }
    
    auto msg = pkt2msg(p);
    auto ret = allocator->on_recv(msg, op_sim_time() * 1e6);

    switch (msg.phase)
    {
    case REQ:
    {
        if (cur_time - time_stamp > (0.036 * 1e6))
        {
            LOG("find over time");
            return;
        }
        if (ret.first == nullptr)
            return;

        beacon_info_t info = msg.info;
        for (int i = 0; i < 34; i++)
        {
            if (info.links[i].command == ALLOCATE)
            {
                if (ret.first->info.links[i].ack == APPROVE)
                {
                    info.links[i].id = id;
                }
                else if (ret.first->info.links[i].ack == DENIEL)
                {
                    info.links[i].id = 0;
                    info.links[i].mode = INVALID;
                    info.links[i].type = VACCANT;
                }
            }
        }
        router.add_vertex(node(info, msg.session));

        if (ret.first != nullptr)
        {
            Packet *p = op_pk_create(0);
            Packet *q = msg2pkt(*ret.first);
            op_pk_fd_set_int32(p, 0, ret.first->dst, 8);
            op_pk_fd_set_pkt(p, 1, q, op_pk_total_size_get(q));
            op_pk_send(p, BACH_DL);
            delete ret.first;
        }

        if (ret.second)
        {
            // LOG("[%d]update self node",id);
            auto bt = allocator->get_beacon_table();
            router.add_vertex(node(bt, allocator::session::get_seq_id()));
        }

        update_beacon_table();
        update_forward_table();
    }
    break;
    case ACK:
    {
        beacon_info_t info = msg.info;
        for (int i = 0; i < 34; i++)
        {
            if (msg.info.links[i].ack == APPROVE)
            {
                info.links[i].id = id;
            }
            else if (msg.info.links[i].ack == DENIEL)
            {
                info.links[i].id = 0;
                info.links[i].type = VACCANT;
                info.links[i].mode = INVALID;
            }
        }
        router.add_vertex(node(info, msg.session));

        if (ret.second)
        {
            // LOG("[%d]update self node",id);
            auto bt = allocator->get_beacon_table();
            router.add_vertex(node(bt, allocator::session::get_seq_id()));
        }

        update_beacon_table();
        update_forward_table();
    }
    break;
    default:
        break;
    }
}

void rrc::proc_link_update(Packet *p)
{
    auto msg = pkt2msg(p);
    auto nb = router.get_vertex(msg.info.id);
    if (nb.second)
    {
        if (nb.first.last_update > msg.session)
            return;
    }

    router.add_vertex(node(msg.info, msg.session));
    update_forward_table();
}

void rrc::update_beacon_table()
{
    {
        //Remove out-dated neighbors and release beacon resources
        // LOGD("BEFORE REMOVE");
        // nodes->remove_if(node::is_out_dated);

        Packet *t = op_pk_create(0);
        node_t self = router.get_vertex(id).first;
        balloc::beacon_info_t info = allocator->get_beacon_table();
        for (int i = 0; i < 34; i++)
        {
            op_pk_fd_set_int32(t, 3 * i, info.links[i].id, 8);
            op_pk_fd_set_int32(t, 3 * i + 1, info.links[i].mode, 8);
            op_pk_fd_set_int32(t, 3 * i + 2, info.links[i].type, 8);
        }
        op_pk_send(t, LCCH_DL);
    }
}

void rrc::update_forward_table()
{

    node_t self = router.get_vertex(id).first;
    router.Dijkstra(self);
    std::map<node_t, node_t> fwd = router.get_forward_table();
    std::set<node_t> mpr = router.get_mpr_nodes();

    link_t fwd_tbl[48] = {0};

    std::set<int> next_hop;
    for (auto n = fwd.begin(); n != fwd.end(); n++)
    {
        fwd_tbl[n->first.get_id() - 1].id = n->second.get_id();
        next_hop.insert(n->second.get_id());
    }

    for (auto n = mpr.begin(); n != mpr.end(); n++)
    {
        fwd_tbl[n->get_id() - 1].MPR = 1;
    }

    for (auto n = next_hop.begin(); n != next_hop.end(); n++)
    {
        fwd_tbl[*n - 1].PRIMARY = 1;
    }

    Packet *p = op_pk_create(0);

    for (int i = 0; i < 48; i++)
    {
        int l = fwd_tbl[i].id | (fwd_tbl[i].MPR << 6) | (fwd_tbl[i].PRIMARY << 7);
        op_pk_fd_set_int32(p, i, l, 8);
    }
    op_pk_send(p, RCCH_DL);
}

void rrc::send_link_broadcast()
{
    Packet *s = generate_link_broadcast();
    op_pk_send(s, LBCH_DL);
}

Packet *rrc::generate_link_broadcast()
{
    balloc::balloc_msg_t msg = allocator->get_hello_msg();
    Packet *s = msg2pkt(msg);
    return s;
}

void rrc::send_link_unicast(int dst)
{
    Packet *p = generate_link_broadcast();
    Packet *q = op_pk_create(0);
    op_pk_fd_set_int32(q, 0, dst, 8);
    op_pk_fd_set_pkt(q, 1, p, op_pk_total_size_get(p));
    op_pk_send(q, LUCH_DL);
}

void rrc::proc_nd(const balloc::balloc_msg_t &msg)
{
    switch (msg.phase)
    {
    case balloc::HELLO:
    {
        auto nb = router.get_vertex(msg.info.id);
        if (nb.second)
        {
            LOG("[%d]<->[%d] = %d", nb.first.get_id(), id, (nb.first | router.get_vertex(id).first));
            if ((nb.first | router.get_vertex(id).first) >= 2)
                break;
        }

        auto ret = allocator->req_beacon(msg.info, balloc::PRIMARY_BALLOC, op_sim_time() * 1e6);

        for (auto i = ret.first.begin(); i != ret.first.end(); ++i)
        {
            balloc::balloc_msg_t reply = i->second;
            Packet *p = op_pk_create_fmt("724nd");
            msg2nd(reply, p);
            op_pk_total_size_set(p, 7500);
            LOG("SEND REQ");
            op_pk_send_quiet(p, NDCH_DL);
        }

        if (ret.second)
        {
            update_beacon_table();
        }
        router.add_vertex(node(allocator->get_beacon_table(), balloc::allocator::session::get_seq_id()));
        router.add_vertex(node(msg.info, msg.session));
        update_forward_table();
    }
    break;
    case balloc::REQ:
    {
        if (msg.dst != id)
            break;

        auto nb = router.get_vertex(msg.info.id);
        auto ret = allocator->on_recv(msg, op_sim_time() * 1e6);
        Packet *p = op_pk_create_fmt("724nd");
        msg2nd(*(ret.first), p);
        op_pk_total_size_set(p, 7500);
        op_pk_send_quiet(p, NDCH_DL);

        balloc::beacon_info_t info = msg.info;
        for (int i = 0; i < 34; i++)
        {
            if (info.links[i].command == balloc::ALLOCATE)
            {
                if (ret.first->info.links[i].ack == balloc::APPROVE)
                {
                    info.links[i].id = id;
                    info.links[i].type = balloc::PRIMARY;
                }
                else
                {

                    info.links[i].id = 0;
                    info.links[i].type = balloc::VACCANT;
                    info.links[i].mode = balloc::INVALID;
                }
            }
        }

        node_t new_nb = node(info, 0);
        router.add_vertex(new_nb);

        delete ret.first;

        if (ret.second)
        {
            update_beacon_table();
        }

        router.add_vertex(node(allocator->get_beacon_table(), balloc::allocator::session::get_seq_id()));
        update_forward_table();
        if(!isstar)
        {
            send_link_broadcast();
        }
    }
    break;
    case balloc::ACK:
    {
        if (msg.dst != id)
            break;
        LOG("[%d]proc ACK from[%d]", id, msg.info.id);
        auto nb = router.get_vertex(msg.info.id);
        if (nb.second)
        {
            LOG("found");
            balloc::beacon_info_t info = nb.first.get_bov();

            auto ret = allocator->on_recv(msg, op_sim_time() * 1e6);

            for (int i = 0; i < 34; i++)
            {
                if (msg.info.links[i].ack == balloc::APPROVE)
                {
                    info.links[i].id = id;
                    info.links[i].type = balloc::PRIMARY;
                    info.links[i].mode = allocator->get_beacon_table().links[i].mode == balloc::SEND ? balloc::RECV : allocator->get_beacon_table().links[i].mode == balloc::RECV ? balloc::SEND : balloc::INVALID;
                }
                else
                {
                    info.links[i].id = 0;
                    info.links[i].type = balloc::VACCANT;
                    info.links[i].mode = balloc::INVALID;
                }
            }

            if (ret.second)
            {
                update_beacon_table();
            }
            router.add_vertex(node(info, 0));
            router.add_vertex(node(allocator->get_beacon_table(), balloc::allocator::session::get_seq_id()));
            update_forward_table();
            if(!isstar)
            {
                send_link_broadcast();
            }
        }
    }
    break;
    default:
        break;
    }
}

void msg2nd(const balloc::balloc_msg_t &msg, Packet *p)
{

    op_pk_fd_set_int32(p, 1, msg.info.id, 8);
    op_pk_fd_set_int32(p, 2, msg.phase, 2);
    op_pk_fd_set_int32(p, 3, msg.dst, 8);
    op_pk_nfd_set_int32(p, "clock", msg.session);
    for (int i = 0; i < 34; i++)
    {

        switch (msg.phase)
        {
        case balloc::HELLO:
            op_pk_fd_set_int32(p, 7 + 3 * i, msg.info.links[i].type == balloc::PRIMARY ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 1, msg.info.links[i].mode == balloc::SEND ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 2, msg.info.links[i].id, 6);
            break;
        case balloc::REQ:
            op_pk_fd_set_int32(p, 7 + 3 * i, msg.info.links[i].command == balloc::ALLOCATE ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 1, msg.info.links[i].mode == balloc::SEND ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 2, msg.info.links[i].id, 6);
            break;
        case balloc::ACK:
            op_pk_fd_set_int32(p, 7 + 3 * i, msg.info.links[i].ack == balloc::APPROVE ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 1, msg.info.links[i].mode == balloc::SEND ? 1 : 0, 1);
            op_pk_fd_set_int32(p, 7 + 3 * i + 2, msg.info.links[i].id, 6);
            break;
        default:
            break;
        }
    }
}

balloc::balloc_msg_t nd2msg(Packet *p)
{
    balloc::balloc_msg msg;
    op_pk_nfd_get_int32(p, "clock", (int *)&msg.session);
    op_pk_fd_get_int32(p, 1, (int *)&msg.info.id);
    op_pk_fd_get_int32(p, 2, (int *)&msg.phase);
    op_pk_fd_get_int32(p, 3, (int *)&msg.dst);
    for (int i = 0; i < 34; i++)
    {
        int rti, ack, id;
        op_pk_fd_get_int32(p, 7 + 3 * i, &ack);
        op_pk_fd_get_int32(p, 7 + 3 * i + 1, &rti);
        op_pk_fd_get_int32(p, 7 + 3 * i + 2, &id);
        switch (msg.phase)
        {
        case balloc::HELLO:
            msg.info.links[i].id = id;
            msg.info.links[i].type = ack ? balloc::PRIMARY : id ? balloc::EXTENDED : balloc::VACCANT;
            msg.info.links[i].mode = rti ? balloc::SEND : id ? balloc::RECV : balloc::INVALID;
            break;
        case balloc::REQ:
            msg.info.links[i].id = id;
            msg.info.links[i].type = ack ? balloc::PRIMARY : id ? balloc::EXTENDED : balloc::VACCANT;
            msg.info.links[i].mode = rti ? balloc::SEND : ack ? balloc::RECV : id ? balloc::RECV : balloc::INVALID;
            msg.info.links[i].command = ack ? balloc::ALLOCATE : balloc::MAINTAIN;
            break;
        case balloc::ACK:
            msg.info.links[i].id = id;
            msg.info.links[i].type = id ? balloc::EXTENDED : balloc::VACCANT;
            msg.info.links[i].mode = rti ? balloc::SEND : id ? balloc::RECV : balloc::INVALID;
            msg.info.links[i].ack = ack ? balloc::APPROVE : balloc::NOP;
            break;
        default:
            break;
        }
    }
    op_pk_destroy(p);
    return msg;
}

Packet *msg2pkt(const balloc::balloc_msg_t &msg)
{
    Packet *p = op_pk_create(0);
    op_pk_fd_set_int32(p, 0, msg.info.id, 8);
    op_pk_fd_set_int32(p, 1, msg.dst, 8);
    op_pk_fd_set_int32(p, 2, msg.phase, 8);
    op_pk_fd_set_int32(p, 3, msg.session, 24);
    for (int i = 0; i < 34; i++)
    {
        op_pk_fd_set_int32(p, 4 * i + 4, msg.info.links[i].command, 4);
        op_pk_fd_set_int32(p, 4 * i + 5, msg.info.links[i].type, 4);
        op_pk_fd_set_int32(p, 4 * i + 6, msg.info.links[i].mode, 4);
        op_pk_fd_set_int32(p, 4 * i + 7, msg.info.links[i].id, 8);
    }
    return p;
}

balloc::balloc_msg_t pkt2msg(Packet *p)
{
    balloc::balloc_msg_t msg;
    memset(&msg, 0x00, sizeof(msg));
    op_pk_fd_get_int32(p, 0, (int *)&msg.info.id);
    op_pk_fd_get_int32(p, 1, (int *)&msg.dst);
    op_pk_fd_get_int32(p, 2, (int *)&msg.phase);
    op_pk_fd_get_int32(p, 3, (int *)&msg.session);
    for (int i = 0; i < 34; i++)
    {
        op_pk_fd_get_int32(p, 4 * i + 4, (int *)&msg.info.links[i].command);
        op_pk_fd_get_int32(p, 4 * i + 5, (int *)&msg.info.links[i].type);
        op_pk_fd_get_int32(p, 4 * i + 6, (int *)&msg.info.links[i].mode);
        op_pk_fd_get_int32(p, 4 * i + 7, (int *)&msg.info.links[i].id);
    }
    op_pk_destroy(p);
    return std::move(msg);
}

} // namespace rrc_ns