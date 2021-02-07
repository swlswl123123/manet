#pragma once
#include <opnet.h>
typedef class state
{
public:
    state()
    {
        Objid obj = op_id_parent(op_id_self());
        op_ima_obj_attr_exists(obj, "user id");
        op_ima_obj_attr_get(obj, "user id", &id);
    }
    virtual bool cond(int cond) = 0;
    virtual void proc(int cond) = 0;

protected:
    int id;
} state_t;