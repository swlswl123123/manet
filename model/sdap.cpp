#pragma once
#include <sdap.hpp>
#include <opnet.h>
#include <log.h>

using namespace sdap_ns;

manager::manager()
{
    //初始化节点id
    obj = op_id_parent(op_id_self());
    if (op_ima_obj_attr_exists(obj, "user id"))
    {
        op_ima_obj_attr_get(obj, "user id", &id);
    }
    else
    {
        LOGD("user id not exits!");
    }
};
manager::~manager(){};

idle::idle(){

};
idle::idle(manager_t *p) : parent(p){};
idle::~idle(){};
void idle::enter(){};
void idle::exit(){};
void idle::proc(int cond)
{
    // LOGD("[%d]SDAP:%s",parent->id,NAME_INTRPT[cond]);
    switch (cond)
    {
    case ON_DL_TRAFFIC:
    {
        Packet *p = op_pk_get(DL);
        // op_pk_print(p);
    }
    case ON_UL_TRAFFIC:
        break;
    default:
        break;
    };
};
bool idle::cond(int cond)
{
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    int strm = op_intrpt_strm();
    switch (code)
    {
    case ON_DL_TRAFFIC:
        return (type == OPC_INTRPT_STRM && strm == DL);
    case ON_UL_TRAFFIC:
        return (type == OPC_INTRPT_STRM && strm == UL);
    default:
        return false;
    }
};