#pragma once
#include <antenna.hpp>
#include <list>
#include <map>
#include <deque>
#include <vector>
#include <node.hpp>
#include <transfer_queue.hpp>
#define OPNET
#include <balloc.hpp>
#include <position_manager.hpp>

#define SCAN_SECTORS 10000 // total scan number multiple slot number of long frame 200*50

#define NUM_SHORT_FRAME_PER_SECOND_DISCON 16
#define NUM_SHORT_FRAME_PER_SECOND_CONN 16
#define NUM_ND_PER_SHORT_FRAME 4
#define NUM_D_PER_SHORT_FRAME 28
#define NUM_D

#define OFFNET_FRAME_INTERVAL 50e-3
#define ASYNC_FRAME_INTERVAL 50e-3
#define INNET_FRAME_INTERVAL 50e-3
#define GAURD_INTERVAL 100e-6
#define TRANSIT_INTERVAL 50e-6
#define ND_INTERVAL 100e-6
#define N_INTERVAL 1e-3
#define D_INTERVAL 1e-3
#define DT_INTERVAL (D_INTERVAL - TRANSIT_INTERVAL - GAURD_INTERVAL)

namespace dtdma_ns
{
typedef enum strm
{
  FROM_TCB,
  TO_TCB,
  RRC_ND,
  RRC_LINK,           //Notifying link state update
  RRC_LINK_BROADCAST, // Broadcasting link infomation
  RRC_FWD_TBL,        // Forward table update interface
  TRAFFIC,            //Traffic interface
  TRAFFIC_URG,        //Urgent traffic
  RRC_BALLOC,         //Beacon allocation interface
  RRC_BALLOC_REQ,     // Beacon allocation request
  RRC_LINK_UNICAST,   //Link info unicast channel
  RRC_RELEASE
} strm_t;

typedef enum intrpt
{
  C,
  ON_NDCH_DL,
  ON_LINK_UPDATE,
  ON_FWD_TBL_UPDATE,
  ON_NETSTATE_UPDATE,
  ON_LBCH_DL,
  ON_TRAFFIC_DL,
  ON_TRAFFIC_URG_DL,
  ON_BALLOC_DL,
  ON_LUCH_DL,
  N = 0x00000100,
  ND = 0x00000200,
  D = 0x00000300,
  DT = 0x00000400,
  DR = 0x00000500,
  SF = 0x00000600

} intrpt_t;
typedef enum ptype
{
  ETHERTYPE_IP = 0x0800,
  ETHERTYPE_ARP = 0x0806,
  ETHERTYPE_IPV6 = 0x86dd,
  EHTERTYPE_BALLOC = 0x7240, //Beacon allocation message type
  EHTERTYPE_RINFO = 0x7241,   //Link info message type
  EHTERTYPE_POS = 0x7242
} ptype_t;

typedef enum prio
{
  DBCH,  //Data broadcast channel
  CBCH,  //Control broadcast channel
  U1,    //Data unicast channel
  U2,    //Data half urgent channel
  CUCH,  //Control unicast channel
  UDBCH, //Urgent data broadcast channel
  UCBCH, //Urgent control broadcast channel
  U3,    //Urgent data unicast channel
  UCUCH  //Urgent control unicast channel
} prio_t;

}; // namespace dtdma_ns

const char *NAME_ND[] = {"ND1_S", "ND1_R", "ND2_S", "ND2_R", "ND3_S", "ND3_R"};

const double FRAME_INTERVAL[] = {ASYNC_FRAME_INTERVAL, OFFNET_FRAME_INTERVAL, INNET_FRAME_INTERVAL};
const int NUM_ND[] = {1, 1, 1};
const int NUM_DT[] = {49, 49, 49};


const int SCAN_ROUND[] = {288, 288, 288};

typedef class opnet_pri_packet : public pri_packet
{
public:
  void *get_packet() const;
  opnet_pri_packet(Packet *p, int prio);
  ~opnet_pri_packet();

private:
  Packet *opnet_packet;

} opnet_pri_packet_t;

typedef class opnet_pri_queue : public transfer_queue
{
  private:
    std::vector<unsigned long> buf_size_by_prio;
  public:
    opnet_pri_queue();
    unsigned long calc_size(dtdma_ns::prio_t prio)const;
    unsigned long calc_size()const;
    void push(const pri_packet_t &p);
    void pop();
  
}opnet_pri_queue_t;

typedef class dtdma
{
private:
  Objid obj;
  node_t *self;
  antenna_t ant;
  int id;
  bool scan_mode[50];
  int cur_scan_sec;
  int status;
  unsigned short scan_count;
  unsigned long long dt_count;
  angle_t scan_seq[2][200];
  vector<int> sec_seq;
  angle_t beam_seq[48];            //Store the sector info of a neighbor
  unsigned long long seq_num[48]; // Store the latest known sequence number of broadcast info
  unsigned long long my_seq_num;  // My broadcasting seq num
  balloc::beacon_t beacon_table[49];
  opnet_pri_queue txQ[48]; //Support maximum 20 transfer queue
  link_t forward_table[48];
  std::deque<Packet *> broadcast_stack;
  std::deque<Packet *> link_info_stack;
  position_manager_t* pos_mngr;
  friend class position_manager;

public:
  dtdma();
  ~dtdma();
  void update_forward_table(Packet *p);
  void update_beacon_table(Packet *p);
  void recv_proc(Packet *p);
  void proc_broadcast(Packet *p);
  void proc_unicast(Packet *p);
  void send_queue_info();
  void send_proc(int d, int dc);
  Packet *encap_mac(Packet *p, int dst, int prio, int type);
  Packet *encap_broadcast(Packet *p);
  friend class idle;

} dtdma_t;