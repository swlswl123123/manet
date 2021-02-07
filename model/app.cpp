#include <app.hpp>
#include <log.h>

using namespace app_ns;

traffic::traffic()
{
}

traffic::~traffic()
{
}

bool traffic::operator==(const traffic_t &t) const
{
    return this->time == t.time;
}
bool traffic::operator>(const traffic_t &t) const
{
    return this->time < t.time;
}

bool traffic::operator<(const traffic_t &t) const
{
    return this->time > t.time;
}

bool app::cond(int cond)
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();
    int stat = op_intrpt_stat();

    switch (cond)
    {
    case ON_RECV:
        return (type == OPC_INTRPT_STRM && (strm == DATA_UL||strm == DATA_URG_UL));
    case ON_SEND:
        return (type == OPC_INTRPT_SELF && (code & 0x80000000) == SEND);
    case ON_BEGSIM:
        return (type == OPC_INTRPT_BEGSIM);
    case ON_ENDSIM:
        return (type == OPC_INTRPT_ENDSIM);
    default:
        return false;
    }
}

void app::proc(int cond)
{
    switch (cond)
    {
    case ON_RECV:
    {
        Packet *p = op_pk_get(op_intrpt_strm());
        if (p == OPC_NIL)
            break;
        double delay = op_sim_time() - op_pk_creation_time_get(p);
        int pkid = 0;
        op_pk_fd_get_int32(p, 0, &pkid);
        LOG("[%d]TRAFFIC:[%d] DELAY = %lf", id, pkid, delay);
        op_pk_destroy(p);
    }
    break;
    case ON_SEND:
    {
        // int index = op_intrpt_code() & 0x00ffffff;
        int size_type = (op_intrpt_code() & 0x60000000) >> 29;
        int urg = (op_intrpt_code() & 0x18000000) >> 27;
        int dst = (op_intrpt_code() & 0x07e00000) >> 21;
        int pkid = (op_intrpt_code() & 0x001fffff);
        
        Packet *p;
        switch (size_type)
        {
        case 0:
            p = op_pk_create(12000);
            break;
        case 1:
            p = op_pk_create(64000);
            break;
        case 2:
            p = op_pk_create(504);
            break;
        case 3:
            p = op_pk_create(2048);
            break;
        default:
            p = op_pk_create(12000);
            break;
        }

        op_pk_fd_set_int32(p, 0, pkid, 32);
        Packet *q = op_pk_create(0);
        op_pk_fd_set_int32(q, 0, dst, 8);
        op_pk_fd_set_int32(q, 1, urg, 8);
        op_pk_fd_set_pkt(q, 2, p, op_pk_total_size_get(p));
        // if (urg)
        // {
        //     LOG("[%d]TRAFFIC:SEND[%d] to [%d] URG %d", id, pkid, dst,urg);
        //     op_pk_send(q, DATA_URG_DL);
        // }
        // else
        // {
        //     LOG("[%d]TRAFFIC:SEND[%d] to [%d] URG %d", id, pkid, dst,urg);
        //     op_pk_send(q, DATA_DL);
        // }
        LOG("[%d]TRAFFIC:SEND[%d] to [%d] URG %d", id, pkid, dst, urg);
        op_pk_send(q, DATA_DL);
    }
    break;
    case ON_BEGSIM:
    {
        FILE *p = fopen("traffic.csv", "r");
        if(p==NULL)
            break;

        traffic_t t;
        while (fscanf(p, "%d,%d,%d,%lf,%d,%d", &t.id, &t.src, &t.dst, &t.time, &t.urg, &t.size) == 6)
        {
            if (t.src != this->id)
                continue;
            // this->traffics.push_back(t);
            int size_type;
            switch (t.size)
            {
            case 12000:
                size_type = 0;
                break;
            case 64000:
                size_type = 1;
                break;
            case 504:
                size_type = 2;
                break;
            case 2048:
                size_type = 3;
                break;
            default:
                break;
            }
            op_intrpt_schedule_self(t.time, SEND | (t.dst) << 21 | t.id | t.urg << 27 | size_type << 29);
        }

        fclose(p);
    }
    break;
    case ON_ENDSIM:
        break;
    }
}