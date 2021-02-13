#include <idle.hpp>
#include <log.h>
#include <log_out.hpp>
#include <stdlib.h>
#include <packet.hpp>
#include <string.h>
#include <stdio.h>

using namespace dtdma_ns;

idle::idle(){};
idle::idle(dtdma_t *proc) : parent(proc), isstar(0), D_pre_type(-1), D_pre_ID(0){};
idle::~idle(){};
bool idle::cond(int cond)
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();

    switch (cond)
    {
    case ON_NDCH_DL:
        return (type == OPC_INTRPT_STRM && strm == RRC_ND);

    case ON_LINK_UPDATE:
        return (type == OPC_INTRPT_STRM && strm == RRC_LINK);

    case ON_FWD_TBL_UPDATE:
        return (type == OPC_INTRPT_STRM && strm == RRC_FWD_TBL);

    case ON_LBCH_DL:
        return (type == OPC_INTRPT_STRM && strm == RRC_LINK_BROADCAST);
    case ON_TRAFFIC_DL:
        return (type == OPC_INTRPT_STRM && strm == TRAFFIC);
    case ON_TRAFFIC_URG_DL:
        return (type == OPC_INTRPT_STRM && strm == TRAFFIC_URG);
    case ON_BALLOC_DL:
        return (type == OPC_INTRPT_STRM && strm == RRC_BALLOC);
    case ON_LUCH_DL:
        return (type == OPC_INTRPT_STRM && strm == RRC_LINK_UNICAST);
    default:
        return false;
    }
};

bool idle::cond()
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();

    if (type == OPC_INTRPT_SELF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void idle::proc(int cond)
{
    switch (cond)
    {
    case ON_NDCH_DL:
    {
        // to send hand shake packets to find neighbor
        LOGD("[%d]proc:NDCH_DL", parent->id);
        Packet *p = op_pk_get(RRC_ND);
        if (p != OPC_NIL)
        {
            op_pk_send(p, TO_TCB);
        }
    }
    break;
    case ON_LINK_UPDATE:
    {
        // get link information from rrc and update beacon table
        LOGD("[%d]mac:ON_LINK_UPDATE", parent->id);
        Packet *p = op_pk_get(RRC_LINK);
        if (p != OPC_NIL)
        {
            for (int i = 0; i < 49; i++)
            {
                int id, rti, type;
                op_pk_fd_get_int32(p, 3 * i, &id);
                op_pk_fd_get_int32(p, 3 * i + 1, &rti);
                op_pk_fd_get_int32(p, 3 * i + 2, &type);
                LOGD("[%d]MAC:ON_LINK_UPDATE:[%d]{id = %d,rti = %d,bearing = %3.2f}", parent->id, i, id, rti, parent->beam_seq[id - 1]);
                parent->beacon_table[i].id = id;
                parent->beacon_table[i].mode = (balloc::mode_t)rti;
                parent->beacon_table[i].type = (balloc::beacon_type_t)type;
            }
            op_pk_destroy(p);
        }
    }
    break;
    case ON_LBCH_DL:
    {
        // broadcast link information
        LOGD("[%d]MAC:ON_LBCH_DL", parent->id);
        Packet *p = op_pk_get(RRC_LINK_BROADCAST);
        p = parent->encap_broadcast(p);
        p = parent->encap_mac(p, 0x3f, UCBCH, EHTERTYPE_RINFO);
        parent->broadcast_stack.push_back(p);
    }
    break;
    case ON_FWD_TBL_UPDATE:
    {
        // update forward table
        LOGD("[%d]MAC:ON_FWD_TBL_UPDATE", parent->id);
        Packet *p = op_pk_get(RRC_FWD_TBL);
        parent->update_forward_table(p);
    }
    break;
    case ON_TRAFFIC_DL:
    {
        // get packets from app layer
        LOG("[%d]MAC:ON_TRAFFIC_DL", parent->id);

        Packet *p = op_pk_get(TRAFFIC);
        int dst = 0, urg;
        if (p == OPC_NIL)
            break;
        op_pk_fd_get_int32(p, 0, &dst);
        op_pk_fd_get_int32(p, 1, &urg);
        Packet *q = OPC_NIL;
        op_pk_fd_get_pkt(p, 2, &q);
        if (q == OPC_NIL)
        {
            op_pk_destroy(p);
            break;
        }

        int urg_type;
        switch (urg)
        {
        case 0: // usual packets
            urg_type = 2;
            break;
        case 2: // a bit urgent
            urg_type = 3;
            break;            
        case 1: // urgent
            urg_type = 7;
            break;            
        default:
            break;
        }

        Packet *r = parent->encap_mac(q, dst, urg_type, ETHERTYPE_IP);
        parent->proc_unicast(r);
        op_pk_destroy(p);
    }
    break;
    case ON_BALLOC_DL:
    {
        // send packets to negotiate slot allocation
        LOG("[%d]ON_BALLOC_DL", parent->id);
        Packet *p = op_pk_get(RRC_BALLOC);
        int dst = 0;
        op_pk_fd_get_int32(p, 0, &dst);
        Packet *q = OPC_NIL;
        op_pk_fd_get_pkt(p, 1, &q);
        op_pk_destroy(p);
        if (q != OPC_NIL)
        {
            Packet *r = parent->encap_mac(q, dst, UCUCH, EHTERTYPE_BALLOC);
            parent->proc_unicast(r);
        }
    }
    break;
    default:
        break;
    }
};

// arrange tdma slot
void idle::proc()
{
    // C slot 
    int cond = op_intrpt_code();
    if (cond == C)  
    {
        // statistic the size tx queue
        for (int i = 0; i < 20; i++)
        {
            // log_write(QUEUE, 3, double(id), double(i), double(parent->txQ[i].size()));
            LOG("[%d]txQ[%d] size  = %lu", parent->id, i, parent->txQ[i].size());
        }

        // statistic the number of occupied slot
        unsigned int B_total = 0;
        for (int i = 0; i < 49; i++)
        {
            if (parent->beacon_table[i].id != 0 && parent->beacon_table[i].type != balloc::PREORDAINED)
            {
                B_total++;
            }
        }
        // log_write(TIME, 2, double(id), double(B_total));
        LOG("[%d]B = %d", id, B_total);

        // statistic number of reachable node
        int reachable = 0;
        for (int i = 0; i < 48; i++)
        {
            if (parent->forward_table[i].id != 0)
                reachable++;
        }
        // log_write(REACH, 2, double(parent->id), double(reachable));
        LOG("[%d]REACHABLE = %d", parent->id, reachable);

        // for single moving node
        // if (id == 17)
        // {
        //     if (reachable != 0)
        //     {
        //         LOG("MOVE NODE INNET");
        //     }
        //     else
        //     {
        //         LOG("MOVE NODE OUTNET");
        //     }
        // }

        // send tx queue size to rrc layer
        parent->send_queue_info();

        // arrange C slot
        op_intrpt_schedule_self(op_sim_time() + FRAME_INTERVAL[parent->status], C);

        // arrange fix scan slot: N0
        for (int i = 0; i < NUM_ND[parent->status]; i++)
        {
            op_intrpt_schedule_self(op_sim_time() + i * N_INTERVAL, N | i);
        }

        // arrange short frame
        for (int i = 0; i < NUM_DT[parent->status]; i++)
        {
            op_intrpt_schedule_self(op_sim_time() + (i + 1) * N_INTERVAL, SF | (i + 1));
        }

        // count the scan sequence number
        if (parent->scan_count == SCAN_SECTORS)
        {
            parent->scan_count = 0;
        }

        // count the main scan sequence, complete one cycle scan
        if (parent->dt_count == 4)
        {
            parent->dt_count = 0;
        }
    
        // arrange the mode of fix scan slot: N0
        if(parent->scan_count = 0 && !isstar)
        {
            bool mode = ((int)op_dist_uniform(2)) == 0 ? false : true;
            parent->scan_mode[0] = mode;
        }

        // arrange modes of other slot: Ni
        if (parent->dt_count == 0 && !isstar)
        {
            bool mode = ((int)op_dist_uniform(2)) == 0 ? false : true;
            for(int i = 1; i < 50; i++)
            {
                parent->scan_mode[i] = mode;
                LOGD("[%d]SCAN_MODE[%d]:%d", parent->id, i, parent->scan_mode);
            }
        }
        parent->dt_count++;
    }
    else
    {
        switch (cond & 0xffffff00)
        {
        // a short frame could be N or D
        case SF:
        {
            int sf = cond & 0x00000ff;
            // send position information every long frame
            if (sf == 1)
            {
                parent->pos_mngr->get_position_send(parent);
            }
            
            // to decide to be N or D
            if(parent->txQ[parent->beacon_table[sf-1].id-1].size() == 0
            && parent->beacon_table[sf-1].mode == balloc::SEND
            || parent->beacon_table[sf-1].mode == balloc::INVALID)
            {
                op_intrpt_schedule_self(op_sim_time(), N | sf);
            } else
            {
                op_intrpt_schedule_self(op_sim_time(), D | (sf-1));
            }
        }
        break;

        case D:
        {
            
            int d = cond & 0x00000ff;
            LOG("[%d]D%d", id, d);
            int d_tmp = d;

            // when the slot setted to be D, jump the scan direction
            parent->scan_count++;
            
            // modify the antanne direction according to position and judge whether link is broken
            if (parent->pos_mngr->modify_beam_angle(d, parent) == 1)
            {
                Packet *p = op_pk_create(0);
                op_pk_fd_set_int32(p, 0, parent->beacon_table[d].id, 8);
                op_pk_send(p, RRC_RELEASE);
                LOG("[%d]send release", id);
            }
            
            //Antenna bearing
            angle_t angle;
            angle.gamma = parent->beam_seq[parent->beacon_table[d].id - 1].gamma;
            angle.alpha = parent->beam_seq[parent->beacon_table[d].id - 1].alpha;
            angle.D = 0;
            parent->ant.execute_pointing(angle);

            // to merge consecutive slots
            // if (parent->beacon_table[d].id != 0)
            // {
            //     if (parent->beacon_table[d].type != balloc::PREORDAINED)
            //     {
            //         if (parent->beacon_table[d].mode == balloc::SEND && (D_pre_type != 0 || D_pre_ID != parent->beacon_table[d].id)) // ID and type of D are all different from pre, we can arrange not continous D
            //         {
            //             op_intrpt_schedule_self(op_sim_time() + TRANSIT_INTERVAL, DT | d);
            //             D_pre_type = 0;
            //             D_pre_ID = parent->beacon_table[d].id;
            //         }
            //         else if (parent->beacon_table[d].mode == balloc::RECV && (d+1 > 48 || parent->beacon_table[d+1].mode != balloc::RECV || parent->beacon_table[d+1].id != parent->beacon_table[d].id)) // compare with after slot
            //         {
            //             op_intrpt_schedule_self(op_sim_time() + (TRANSIT_INTERVAL + GAURD_INTERVAL + DT_INTERVAL) - 1e-6, DR | d);
            //             D_pre_type = 1;
            //             D_pre_ID = parent->beacon_table[d].id;
            //         }
            //     }
            // }

            if (parent->beacon_table[d].id != 0)
            {
                if (parent->beacon_table[d].type != balloc::PREORDAINED)
                {
                    if (parent->beacon_table[d].mode == balloc::SEND) 
                    {
                        op_intrpt_schedule_self(op_sim_time() + TRANSIT_INTERVAL, DT | d);
                    }
                    else if (parent->beacon_table[d].mode == balloc::RECV) 
                    {
                        op_intrpt_schedule_self(op_sim_time() + (TRANSIT_INTERVAL + GAURD_INTERVAL + DT_INTERVAL) - 1e-6, DR | d);
                    }
                }
            }

            // send broadcast packets
            while (parent->broadcast_stack.size() > 0)
            {
                Packet *n = parent->broadcast_stack.front();
                int dst = 0;
                op_pk_nfd_get_int32(n, "dst", &dst);
                if (dst == 0x3f)
                {
                    parent->proc_broadcast(n);
                }
                else
                {
                    parent->proc_unicast(n);
                }
                op_pk_destroy(n);
                parent->broadcast_stack.pop_front();
            }
        }
        break;
        case DT:
        {
            LOG("[%d]DT%d ant = %.2f", parent->id, cond & 0x00000ff, parent->beam_seq[parent->beacon_table[cond & 0x000000ff].id - 1].gamma);
            int d = cond & 0x000000ff;
            int d_index = d;
            int num_continue_D = 1;
            // statistic num of consecutive D slot
            // while(d_index + 1 < 49 && parent->beacon_table[d_index].id == parent->beacon_table[d_index+1].id && parent->beacon_table[d_index].type == balloc::SEND && parent->beacon_table[d_index+1].type == balloc::SEND)
            // {
            //     num_continue_D++;
            //     d_index++;
            // }
            if (parent->beacon_table[d].mode == balloc::SEND && parent->beacon_table[d].type != balloc::PREORDAINED && parent->beacon_table[d].type != balloc::VACCANT)
            {
                LOG("[%d]num_continue_D:start %d last %d", parent->id, d, num_continue_D);
                parent->send_proc(d, num_continue_D);
            }
        }
        break;

        case DR:
        {
            LOG("[%d]DR%d ant = %.2f degree bid = %d", parent->id, cond & 0x00000ff, parent->beam_seq[parent->beacon_table[cond & 0x000000ff].id - 1].gamma,parent->beacon_table[cond & 0x000000ff].id);
            int d = cond & 0x000000ff;
            LOGD("[%d]DR%d", parent->id, d);
            //Receiving
            if (parent->beacon_table[d].mode == balloc::RECV)
            {
                parent->recv_proc(op_pk_get(FROM_TCB));
            }
        }
        break;
        case N:
        {
            op_pk_get(FROM_TCB);
            int ndid = cond & 0x000000ff;
            D_pre_type = -1;
            //Set antenna bearing
            angle_t angle;
            angle.alpha = parent->scan_seq[parent->scan_mode[ndid]? 0 : 1][parent->sec_seq[parent->scan_count]].alpha;
            angle.gamma = parent->scan_seq[parent->scan_mode[ndid]? 0 : 1][parent->sec_seq[parent->scan_count]].gamma;
            LOG("[%d]N%d alpha = %f,gamma = %f", parent->id, (cond & 0x00000ff),angle.alpha,angle.gamma);
            parent->cur_scan_sec = ndid;
            parent->ant.execute_pointing(angle);
            for (int i = 0; i < 3; i++) // three hand shake
            {
                op_intrpt_schedule_self(op_sim_time() + i * N_INTERVAL/3 + TRANSIT_INTERVAL, ND | (2 * i));
                op_intrpt_schedule_self(op_sim_time() + (i + 1) * N_INTERVAL/3 - 1e-6, ND | (2 * i + 1));
            }
            parent->scan_count++;
        }
            break;
        case ND:
        {
            LOG("[%d]%s", parent->id, NAME_ND[cond & 0x000000ff]);

            // nd 0 1 2 3 4 5
            // 0    r s     r
            // 1  s     r s
            if (cond % 2 == 0)
            {

                //Request ND1 packets from RRC
                if (parent->scan_mode[parent->cur_scan_sec] && cond == (ND | 0x00))
                {
                    op_strm_access(RRC_ND);
                }
                else if (!parent->scan_mode[parent->cur_scan_sec] && cond == (ND | 0x02))
                {
                    // Fetch ND2 from RRC & send
                    Packet *p = op_pk_get(RRC_ND);
                    if (p != OPC_NIL)
                    {
                        op_pk_send(p, TO_TCB);
                    }
                }
                else if (parent->scan_mode[parent->cur_scan_sec] && cond == (ND | 0x04))
                {
                    // Fetch ND3 from RRC & send
                    Packet *p = op_pk_get(RRC_ND);
                    if (p != OPC_NIL)
                    {
                        op_pk_send(p, TO_TCB);
                    }
                }
            }
            else
            {
                if (parent->scan_mode[parent->cur_scan_sec] && cond == (ND | 0x03) || (!parent->scan_mode[parent->cur_scan_sec] && (cond == (ND | 0x01) || cond == (ND | 0x05))))
                {
                    // op_strm_access(FROM_TCB);
                    Packet *p = op_pk_get(FROM_TCB);
                    while(op_strm_pksize(FROM_TCB))
                    {
                        op_pk_get(FROM_TCB);
                    }
                    //todo: filter out noise packets

                    if (p != OPC_NIL)
                    {
                        int size = op_pk_total_size_get(p);
                        if (size != 7500)
                        {
                            op_pk_destroy(p);
                            p = OPC_NIL;
                            break;
                        }
                        // LOGD("------------------------%p",p);
                        // op_pk_print(p);
                        int prev;
                        int type;
                        int dst;
                        op_pk_fd_get_int32(p, 1, &prev);
                        op_pk_fd_get_int32(p,2,&type);
                        op_pk_fd_get_int32(p,3,&dst);
                        LOG("[%d]type = %d prev = %d dst = %d",parent->id, type, prev, dst);

                        if(cond == (ND | 0x01) && type == balloc::HELLO
                        || cond == (ND | 0x03) && type == balloc::REQ
                        || cond == (ND | 0x05) && type == balloc::ACK)
                        {
                            parent->beam_seq[prev - 1].gamma = parent->ant.pointing.gamma;
                            parent->beam_seq[prev - 1].alpha = parent->ant.pointing.alpha;
                        }
                        op_pk_send(p, RRC_ND);
                    }
                }
            }
        }
        break;
        default:
            break;
        }
    }
};
void idle::enter()
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    if (type == OPC_INTRPT_BEGSIM)
    {
        op_intrpt_schedule_self(op_sim_time(), C);
    }
};
void idle::exit()
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();
    // LOGD("[%d]mac:IDLE:exit:type = %d,code = %d,strm = %d",parent->id,type,code,strm);
};
