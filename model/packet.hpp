#pragma once

// #define BIT_PER_UNIT 341250
#define BIT_PER_UNIT 450000
typedef enum nstate
{
    ASYNC,
    OFFNET,
    INNET
} nstate_t;

typedef enum
{
    COMP_PHY,
    COMP_MAC,
    COMP_RLC,
    COMP_PDCP,
    COMP_SDAP,
    COMP_RRC
} comp_addr_t;

typedef struct
{
    comp_addr_t src;
    comp_addr_t dst;
    unsigned char *data;
} cmd_t;

typedef enum
{
    ND1_S,
    ND1_R,
    ND2_S,
    ND2_R,
    ND3_S,
    ND3_R
} mac_cmd_type;

typedef struct
{
    mac_cmd_type type;
    unsigned int len;
    unsigned char *data;
} mac_cmd_t;

typedef struct
{
    union {
        bool REQ : 1;
        bool ACK : 1;
        bool PRIMARY : 1;
    };
    union {
        bool MPR : 1;
        bool RTI : 1;
    };
    unsigned char id : 6;
} link_t;

typedef struct
{

    //Source node
    unsigned char prev : 6;
    unsigned char state : 2; //state: 0 - ASYNC 1 - Offnet 2 - Innet 3 - Reserved

    //Dest node
    unsigned char next : 6;
    unsigned char type : 2; //type: 0 - HELLO 1 - REQ 2 - REPLY 3 - BROADCAST

    bool rsv : 1;
    bool ext_ref : 1;
    unsigned char ref_clock_id : 6;
    unsigned char ref_clock[8];

    link_t link[124];

} nd_t;

//Route info of current node 48bytes 384bits
typedef struct
{
    unsigned long long seq_num; // Sequence number of route info
    link_t links[124];
} rinfo_t;

typedef class pri_packet
{
protected:
    unsigned long size;
    unsigned char priority;
    void *data;

public:
    virtual bool operator==(const pri_packet &p) const;
    virtual bool operator>(const pri_packet &p) const;
    virtual bool operator<(const pri_packet &p) const;
    virtual unsigned long get_size() const;
    virtual unsigned char get_priority() const;
    virtual void *get_packet() const;
} pri_packet_t;