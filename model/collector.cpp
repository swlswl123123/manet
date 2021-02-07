#include <opnet.h>
#include <log.h>
#include <collector.hpp>

using namespace collector_ns;

collector::collector()
{
    Objid obj = op_id_parent(op_id_self());
    if (op_ima_obj_attr_exists(obj, "user id"))
    {
        op_ima_obj_attr_get(obj, "user id", &id);
    }
    else
    {
        LOGD("user id not exits!");
    }
}

collector::~collector()
{
}

void collector::enter()
{
}
void collector::exit()
{
}
bool collector::cond(int cond)
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();
    int stat = op_intrpt_stat();

    switch (cond)
    {
    case ON_DELAY_UPDATE:
        return (type == OPC_INTRPT_STAT && stat == DELAY_UPDATE);
    case ON_BIT_RATE_UPDATE:
        return (type == OPC_INTRPT_STAT && stat == BIT_RATE_UPDATE);
    default:
        return false;
    }
}
void collector::proc(int cond)
{
    switch (cond)
    {
    case ON_DELAY_UPDATE:
    {
        LOGD("[%d]ON_DELAY_UPDATE", id);
        double delay = op_stat_read(DELAY_UPDATE);
        LOG("[%d] DELAY = %3.6lf", id, delay);
    }
    break;
    case ON_BIT_RATE_UPDATE:
    {
        double bit_rate = op_stat_read(BIT_RATE_UPDATE);
    }
    break;
    default:
        break;
    }
}