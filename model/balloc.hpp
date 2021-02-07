#pragma once
#include <map>
#include <vector>
#include<log.h>

namespace balloc
{

typedef enum phase
{
    HELLO,
    REQ,
    ACK,
} phase_t;

typedef enum mode
{
    INVALID,
    RECV,
    SEND
} mode_t;

typedef enum beacon_type
{
    VACCANT,     //Indicate current beacon is vaccant
    PREORDAINED, //Indicate current beacon is pre-ordained
    EXTENDED,    //Indicate current beacon is occupied as extended beacon i.e. can be removed as required
    PRIMARY,     //Indicate current beacon is occupied as primary beacon
    URGENT       //Indicate current beacon is occupied as urgent beacon i.e. can only be released by the allocator
} beacon_type_t;

typedef enum session_type
{
    EXTENDED_BALLOC,
    PRIMARY_BALLOC,
    URGENT_BALLOC
} session_type;

typedef enum cmd
{
    MAINTAIN,
    ALLOCATE,
    RELEASE,
} cmd_t;

typedef enum ack
{
    NOP,
    APPROVE,
    DENIEL
} ack_t;

typedef struct beacon
{
    union {
        cmd_t command;
        ack_t ack;
    };
    mode_t mode;
    beacon_type_t type;
    unsigned int id;
} beacon_t;

typedef struct beacon_info
{
    unsigned int id;
    beacon_t links[34];
} beacon_info_t;

typedef struct balloc_msg //beacon allocation message 90byte
{
    unsigned int dst;
    phase_t phase;
    unsigned int session;
    beacon_info_t info;
} balloc_msg_t;

class allocator
{

public:
    allocator(unsigned int id); //Constructor of beacon allocator, the only thing it should know is id
    ~allocator();
    const std::pair<balloc_msg *, bool> on_recv(const balloc_msg_t &ba, unsigned long long clock = 0);
    const balloc_msg get_hello_msg() const;

    const std::pair<std::map<int, balloc_msg_t>, bool> req_beacon(const beacon_info &neighbor, const session_type type, unsigned long long clock = 0, int num = 0);
    const beacon_info_t get_beacon_table() const;
    void release(int d);
    typedef class session
    {
    public:
        session(int dst, const beacon_info_t &req, session_type type, unsigned long long clock = 0, unsigned long long exp = 0);
        session(unsigned int session_id); //this constructor is only used for searching
        const beacon_info_t &get_req() const;
        bool shoud_be_removed(unsigned long long clock = 0) const;
        bool operator<(const session &s) const;
        bool operator==(const session &s) const;
        bool operator>(const session &s) const;
        unsigned int get_session_id() const;
        void proc_timeout();
        int get_dst() const;
        static unsigned int get_seq_id();

    private:
        session_type type;
        unsigned int session_id;
        unsigned long long create_time;
        unsigned long long expire;
        beacon_info_t req;
        static unsigned int seq;
        int dst;
    } session_t;

protected:
    const std::pair<std::map<int, beacon_info_t>, bool> req_prim_beacon(const beacon_info_t &ba);
    const std::pair<std::map<int, beacon_info_t>, bool> req_ext_beacon(const beacon_info_t &ba, int num);
    const std::pair<std::map<int, beacon_info_t>, bool> req_urg_beacon(const beacon_info_t &ba);
    const std::pair<beacon_info_t, bool> handle_req(const beacon_info_t &ba);
    bool handle_ack(const session_t &s, const beacon_info_t &ba);
    beacon_info_t beacons;

    std::vector<session> sessions;
};

} // namespace balloc