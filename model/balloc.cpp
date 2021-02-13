#include <opnet.h>
#include <balloc.hpp>
#include <string.h>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

namespace balloc
{

const char *string_type[] = {"VACCANT", "PREORDAINED", "EXTENDED", "PRIMARY", "URGENT"};
const char *string_mode[] = {"INVALID", "RECV", "SEND"};
const char *string_cmd[] = {"MAINTTAIN", "ALLOCATE", "RELEASE"};
const char *string_ack[] = {"NOP", "APPROVE", "DENIAL"};

unsigned int allocator::session::seq = 1;

allocator::session::session(unsigned int session_id)
{
    this->session_id = session_id;
}

allocator::session::session(int dst, const beacon_info_t &req, session_type type, unsigned long long clock, unsigned long long exp)
{

    this->session_id = seq++;
    this->type = type;
    this->create_time = clock;
    this->req = req;
    this->dst = dst;
    this->expire = exp;
};

const std::pair<beacon_info_t, bool> allocator::handle_req(const beacon_info_t &ba)
{
    printf("[%d]<-[%d]REQ\n", beacons.id, ba.id);
    auto ret = std::pair<beacon_info_t, bool>();
    ret.first = beacons;
    ret.second = false;

    for (int i = 0; i < 49; i++)
    {
        printf("[%d]handle_req:[%d]{id = %d mode = %s type = %s cmd = %s}\n",
               beacons.id, i, ba.links[i].id,
               string_mode[(unsigned int)(ba.links[i].mode)],
               string_type[(unsigned int)(ba.links[i].type)],
               string_cmd[(unsigned int)(ba.links[i].command)]);

        switch (ba.links[i].command)
        {
        case ALLOCATE:
            if (beacons.links[i].type == VACCANT)
            {
                //Approve
                beacons.links[i].id = ba.id;
                beacons.links[i].mode = ba.links[i].mode == SEND ? RECV : SEND;
                beacons.links[i].type = ba.links[i].type;
                ret.second = true;
                ret.first.links[i].ack = APPROVE;
                ret.first.links[i].type = beacons.links[i].type;
                ret.first.links[i].mode = beacons.links[i].mode;
                ret.first.links[i].id = beacons.links[i].id;
            }
            else
            {
                //Denial
                ret.first.links[i].ack = DENIEL;
            }
            break;
        case RELEASE:
        {
            if (beacons.links[i].id == ba.id)
            {
                beacons.links[i].id = 0;
                beacons.links[i].mode = INVALID;
                beacons.links[i].type = VACCANT;

                ret.first.links[i].ack = APPROVE;
                ret.first.links[i].type = VACCANT;
                ret.first.links[i].mode = INVALID;
                ret.second = true;
            }
            else
            {
                ret.first.links[i].ack = DENIEL;
            }
        }
        break;
        case MAINTAIN:
            ret.first.links[i].ack = NOP;
            break;
        default:
            break;
        }
    }

    return std::move(ret);
}

bool allocator::handle_ack(const session &s, const beacon_info_t &ba)
{
    printf("[%d]<-[%d]ACK\n", beacons.id, ba.id);
    bool ret = false;

    auto req = s.get_req();

    for (int i = 0; i < 49; i++)
    {
        printf("[%d]handle_ack:[%d]{id = %d mode = %s type = %s ack = %s sessionid = %d}\n",
               beacons.id, i, ba.links[i].id,
               string_mode[(unsigned int)(ba.links[i].mode)],
               string_type[(unsigned int)(ba.links[i].type)],
               string_ack[(unsigned int)(ba.links[i].ack)],
               s.get_session_id());
        switch (req.links[i].command)
        {

        case ALLOCATE:
            if (ba.links[i].ack == APPROVE)
            {
                if (beacons.links[i].type == PREORDAINED)
                {
                    beacons.links[i].id = ba.id;
                    beacons.links[i].mode = req.links[i].mode;
                    beacons.links[i].type = req.links[i].type;
                    ret = true;
                }
            }
            else if (ba.links[i].ack == DENIEL)
            {
                if (beacons.links[i].type == PREORDAINED)
                {
                    beacons.links[i].id = 0;
                    beacons.links[i].type = VACCANT;
                    beacons.links[i].mode = INVALID;
                    ret = true;
                }
            }
            break;
        case RELEASE:
            if (ba.links[i].ack == APPROVE)
            {
                if (beacons.links[i].id == ba.id)
                {
                    beacons.links[i].id = 0;
                    beacons.links[i].type = VACCANT;
                    beacons.links[i].mode = INVALID;
                    ret = true;
                }
            }
            break;
        default:
            break;
        }
    }

    return ret;
}
bool allocator::session::operator<(const session &s) const
{
    return this->session_id < s.session_id;
}

bool allocator::session::operator==(const session &s) const
{
    return this->session_id == s.session_id;
}

bool allocator::session::operator>(const session &s) const
{
    return this->session_id > s.session_id;
}

bool allocator::session::shoud_be_removed(unsigned long long clock) const
{
    printf("session_id = %d create_time = %lu, clock = %lu\n", get_session_id(), create_time, clock);
    return (clock - create_time) > expire;
}

const balloc_msg allocator::get_hello_msg() const
{
    balloc_msg ret;
    memset(&ret, 0x00, sizeof(ret));
    ret.info = beacons;
    ret.phase = HELLO;
    ret.dst = 0x3f;
    ret.session = session::get_seq_id();
    return std::move(ret);
}

allocator::allocator(unsigned int id)
{
    memset(&this->beacons, 0x00, sizeof(this->beacons));
    this->beacons.id = id;
}

allocator::~allocator()
{
}

const std::pair<balloc_msg *, bool> allocator::on_recv(const balloc_msg &ba, unsigned long long clock)
{
    auto ret = std::pair<balloc_msg_t *, bool>();
    ret.first = nullptr;
    ret.second = false;

    for (auto i = sessions.begin(); i != sessions.end();)
    {
        // check whether some req expired, any time can check
        if (i->shoud_be_removed(clock))
        {
            printf("[%d]should_be_removed session_id = %d dst = %d\n", beacons.id, i->get_session_id(), i->get_dst());
            auto req = i->get_req();
            for (int j = 0; j < 49; j++)
            {
                if (req.links[j].command != MAINTAIN)
                {
                    if (beacons.links[j].type == PREORDAINED)
                    {
                        beacons.links[j].type = VACCANT;
                        beacons.links[j].id = 0;
                        beacons.links[j].mode = INVALID;
                    }
                }
            }
            sessions.erase(i);
        }
        else
        {
            ++i;
        }
    }

    switch (ba.phase)
    {
    case REQ: //Process rightaway
    {
        auto r = handle_req(ba.info);

        ret.first = new balloc_msg_t;
        ret.first->dst = ba.info.id;
        ret.first->phase = ACK;
        ret.first->session = ba.session;
        ret.first->info = std::move(r.first);
        ret.second = r.second;
    }
    break;
    case ACK: //Check for session table
    {
        auto i = std::find(sessions.begin(), sessions.end(), session(ba.session));
        if (i == sessions.end())
        {
            printf("session_not found! id = %d\n", ba.session);
            break;
        }
        ret.second = handle_ack(*i, ba.info);
        sessions.erase(i);
    }
    break;
    default:
        break;
    }
    return ret;
}

const std::pair<std::map<int, balloc_msg_t>, bool> allocator::req_beacon(const beacon_info &neighbor, const session_type type, unsigned long long clock, int num)
{

    printf("req_beacon\n");
    auto ret = std::pair<std::map<int, balloc_msg_t>, bool>(std::map<int, balloc_msg_t>(), false);

    for (auto i = sessions.begin(); i < sessions.end();)
    {
        if (i->shoud_be_removed(clock))
        {

            printf("[%d]should_be_removed session_id = %d dst = %d\n", beacons.id, i->get_session_id(), i->get_dst());
            auto req = i->get_req();
            for (int j = 0; j < 49; j++)
            {
                if (req.links[j].command != MAINTAIN)
                {
                    if (beacons.links[j].type == PREORDAINED)
                    {
                        beacons.links[j].type = VACCANT;
                        beacons.links[j].id = 0;
                        beacons.links[j].mode = INVALID;
                    }
                }
            }
            sessions.erase(i);
        }
        else
        {
            ++i;
        }
    }

    switch (type)
    {
    case PRIMARY_BALLOC:
    {
        auto r = req_prim_beacon(neighbor);
        ret.second = r.second;
        for (auto i = r.first.begin(); i != r.first.end(); i++)
        {

            session_t s(neighbor.id, i->second, type, clock, 0.001 * 1e6);
            balloc_msg_t msg;
            memset(&msg, 0x00, sizeof(msg));
            msg.dst = i->first;
            msg.phase = REQ;
            msg.session = s.get_session_id();
            msg.info = std::move(i->second);
            ret.first.insert(std::make_pair(i->first, std::move(msg)));
            printf("[%d]->[%d]req_prim session_id = %d\n", beacons.id, msg.dst, s.get_session_id());
            sessions.push_back(s);
        }
    }
    break;
    case EXTENDED_BALLOC:
    {
        for (auto i = sessions.begin(); i != sessions.end(); ++i)
        {
            if (i->get_dst() == neighbor.id)
            {
                return ret;
            }
        }

        auto r = req_ext_beacon(neighbor, num);
        ret.second = r.second;
        for (auto i = r.first.begin(); i != r.first.end(); i++)
        {

            session_t s(neighbor.id, i->second, type, clock, 0.1 * 1e6);       // after two frame (100ms), it is expired
            balloc_msg_t msg;
            memset(&msg, 0x00, sizeof(msg));    // clear               
            msg.dst = i->first;
            msg.phase = REQ;
            msg.session = s.get_session_id();   // once session is created, there will be new id
            msg.info = std::move(i->second);
            ret.first.insert(std::make_pair(i->first, std::move(msg)));
            sessions.push_back(s);
        }
    }
    break;
    case URGENT_BALLOC:
    {
        for (auto i = sessions.begin(); i != sessions.end(); ++i)
        {
            if (i->get_dst() == neighbor.id)
            {
                return ret;
            }
        }

        auto r = req_urg_beacon(neighbor);
        ret.second = r.second;

        for (auto i = r.first.begin(); i != r.first.end(); i++)
        {
            session_t s(neighbor.id, i->second, type, clock, 0.036/18.0f * 1e6);
            balloc_msg_t msg;
            memset(&msg, 0x00, sizeof(msg));
            msg.dst = i->first;
            msg.phase = REQ;
            msg.session = s.get_session_id();
            msg.info = std::move(i->second);
            ret.first.insert(std::make_pair(i->first, std::move(msg)));
            sessions.push_back(s);
        }
    }
    break;
    default:
        break;
    }
    return ret;
}

const std::pair<std::map<int, beacon_info_t>, bool> allocator::req_prim_beacon(const beacon_info_t &ba)
{
    LOG("[%d]->[%d]req_base_beacon", beacons.id, ba.id);
    auto ret = std::pair<std::map<int, beacon_info_t>, bool>();
    ret.first = std::map<int, beacon_info_t>();
    ret.second = false;

    std::vector<int> vaccum;
    int has_tx = 0;
    int has_rx = 0;
    for (int i = 0; i < 49; i++)
    {
        if (beacons.links[i].type == VACCANT && ba.links[i].type == VACCANT)
        {
            vaccum.push_back(i);
        }

        if (beacons.links[i].type == PRIMARY && beacons.links[i].id == ba.id)
        {
            if (beacons.links[i].mode == SEND)
            {
                has_tx = 1;
            }
            else if (beacons.links[i].mode == RECV)
            {
                has_rx = 1;
            }
        }
    }

    if (vaccum.size() < 1 || has_tx && has_rx)
        return ret;

    ret.second = true;
    beacon_info_t req = beacons;

    // std::random_shuffle(vaccum.begin(), vaccum.end());
    // std::random_shuffle(vaccum_send.begin(), vaccum_send.end());
    // std::random_shuffle(vaccum_receive.begin(), vaccum_receive.end());

    if (!has_rx || !has_tx)
    {
        for (auto i = vaccum.begin(); i != vaccum.end(); ++i)
        {
            if (!has_tx)
            {
                beacons.links[*i].type = PREORDAINED;
                req.links[*i].mode = SEND;
                req.links[*i].type = PRIMARY;
                req.links[*i].command = ALLOCATE;
                has_tx = 1;
                continue;
            }
            if (!has_rx)
            {
                beacons.links[*i].type = PREORDAINED;
                req.links[*i].mode = RECV;
                req.links[*i].type = PRIMARY;
                req.links[*i].command = ALLOCATE;
                has_rx = 1;
                continue;
            }

        }
    }

    for (int i = 0; i < 49; i++)
    {
        printf("[%d]req_prim_beacon:[%d]{id = %d mode = %s type = %s cmd = %s}\n",
               beacons.id, i, req.links[i].id,
               string_mode[(unsigned int)(req.links[i].mode)],
               string_type[(unsigned int)(req.links[i].type)],
               string_cmd[(unsigned int)(req.links[i].command)]);
    }

    ret.first.insert(std::make_pair(ba.id, req));

    return ret;
}

const std::pair<std::map<int, beacon_info_t>, bool> allocator::req_ext_beacon(const beacon_info_t &ba, int num)
{
    printf("[%d]->[%d]req_ext_beacon num = %d\n", beacons.id, ba.id, num);
    auto ret = std::pair<std::map<int, beacon_info_t>, bool>();
    ret.first = std::map<int, beacon_info_t>();
    ret.second = false;
    if (num == 0)
    {
        printf("[%d]req_ext_beacon num == 0\n", beacons.id);
        return ret;
    }

    std::vector<int> vaccum;
    std::vector<int> ext_ocup;

    for (int i = 0; i < 49; i++)
    {
        if (beacons.links[i].type == VACCANT && beacons.links[i].mode == INVALID && ba.links[i].type == VACCANT)
        {
            vaccum.push_back(i);
        }

        if (beacons.links[i].type == EXTENDED && beacons.links[i].id == ba.id && beacons.links[i].mode == SEND)
        {
            ext_ocup.push_back(i);
        }
    }

    beacon_info_t req = beacons;

    // std::random_shuffle(vaccum.begin(), vaccum.end());
    // std::random_shuffle(vaccum_send.begin(), vaccum_send.end());
    // std::random_shuffle(ext_ocup.begin(), ext_ocup.end());

    auto set = num > 0 ? vaccum : ext_ocup;

    if (set.size() == 0)
    {
        return ret;
    }

    for (int i = 0; i < set.size() && i < (num > 0 ? num : -num); i++)
    {
        int index = set[i];

        if (num > 0)
        {
            beacons.links[index].type = PREORDAINED;
            beacons.links[index].id = ba.id;
            beacons.links[index].mode = INVALID;

            req.links[index].type = EXTENDED;
            req.links[index].id = 0;
            req.links[index].mode = SEND;
            req.links[index].command = ALLOCATE;
        }
        else
        {
            beacons.links[index].type = VACCANT;
            beacons.links[index].id = 0;
            beacons.links[index].mode = INVALID;

            req.links[index].type = VACCANT;
            req.links[index].id = 0;
            req.links[index].mode = INVALID;
            req.links[index].command = RELEASE;
        }

        ret.second = true;
    }

    for (int i = 0; i < 49; i++)
    {
        printf("[%d]req_ext_beacon:[%d]{id = %d mode = %s type = %s cmd = %s}\n",
               beacons.id, i, req.links[i].id,
               string_mode[(unsigned int)(req.links[i].mode)],
               string_type[(unsigned int)(req.links[i].type)],
               string_cmd[(unsigned int)(req.links[i].command)]);
    }

    ret.first.insert(std::make_pair(ba.id, req));

    return ret;
}
const std::pair<std::map<int, beacon_info_t>, bool> allocator::req_urg_beacon(const beacon_info_t &ba)
{
    printf("[%d]->[%d]req_urg_beacon", beacons.id, ba.id);
    auto ret = std::pair<std::map<int, beacon_info_t>, bool>();
    ret.first = std::map<int, beacon_info_t>();
    ret.second = false;

    // std::vector<int> vaccum;
    // std::vector<int> ext_ocup;

    // for (int i = 0; i < 124; i++)
    // {
    //     if (beacons.links[i].type == VACCANT)
    //     {
    //         vaccum.push_back(i);
    //     }

    //     if (beacons.links[i].type == EXTENDED && beacons.links[i].id == ba.id && beacons.links[i].mode == SEND)
    //     {
    //         ext_ocup.push_back(i);
    //     }
    // }

    // beacon_info_t req = beacons;

    // std::random_shuffle(vaccum.begin(), vaccum.end());
    // std::random_shuffle(ext_ocup.begin(), ext_ocup.end());

    // auto set = ext_ocup;//num > 0 ? vaccum : ext_ocup;

    // if (set.size() == 0)
    // {
    //     printf("[%d]req_ext_beacon set.size == 0\n");
    //     return ret;
    // }

    // for (int i = 0; i < set.size() && i < (num > 0 ? num : -num); i++)
    // {
    //     int index = set[i];

    //     if (num > 0)
    //     {
    //         beacons.links[index].type = PREORDAINED;
    //         beacons.links[index].id = ba.id;
    //         beacons.links[index].mode = INVALID;

    //         req.links[index].type = EXTENDED;
    //         req.links[index].id = 0;
    //         req.links[index].mode = SEND;
    //         req.links[index].command = ALLOCATE;
    //     }
    //     else
    //     {
    //         beacons.links[index].type = VACCANT;
    //         beacons.links[index].id = 0;
    //         beacons.links[index].mode = INVALID;

    //         req.links[index].type = VACCANT;
    //         req.links[index].id = 0;
    //         req.links[index].mode = INVALID;
    //         req.links[index].command = RELEASE;
    //     }

    //     ret.second = true;
    // }

    // for (int i = 0; i < 124; i++)
    // {
    //     printf("[%d]req_ext_beacon:[%d]{id = %d mode = %s type = %s cmd = %s}\n",
    //            beacons.id, i, req.links[i].id,
    //            string_mode[(unsigned int)(req.links[i].mode)],
    //            string_type[(unsigned int)(req.links[i].type)],
    //            string_cmd[(unsigned int)(req.links[i].command)]);
    // }

    // ret.first.insert(std::make_pair(ba.id, req));

    return ret;
}

unsigned int allocator::session::get_session_id() const
{
    return session_id;
}

const beacon_info_t &allocator::session::get_req() const
{
    return req;
}

void allocator::session::proc_timeout()
{
}

unsigned int allocator::session::get_seq_id()
{
    return seq++;
}
const beacon_info_t allocator::get_beacon_table() const
{
    beacon_info_t info = beacons;
    for (int i = 0; i < 49; i++)
        printf("[%d]beacon_table:[%d]{id = %d mode = %s(%d) type = %s}\n", info.id, i, beacons.links[i].id, string_mode[(unsigned int)(beacons.links[i].mode)], (unsigned int)beacons.links[i].mode, string_type[(unsigned int)(beacons.links[i].type)]);
    return std::move(info);
}

int balloc::allocator::session::get_dst() const
{
    return dst;
}

void balloc::allocator::release(int d)
{
    for (int i = 0; i < 49; i++)
    {
        if (beacons.links[i].id == d)
        {
            beacons.links[i].id = 0;
            beacons.links[i].mode = INVALID;
            beacons.links[i].type = VACCANT;
        }
    }
}

}; // namespace balloc
