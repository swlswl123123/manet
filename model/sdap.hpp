#pragma once
#include <state.hpp>

namespace sdap_ns{

typedef enum{
    ON_DL_TRAFFIC,ON_UL_TRAFFIC
}intrpt_t;

static const char * NAME_INTRPT[] = {"ON_DL_TRAFFIC","ON_UL_TRAFFIC"};

typedef enum{
    DL,UL
}strm_t;

typedef class manager{
    public:
        manager();
        ~manager();
    private:
        Objid obj;
        int id;

    friend class idle;
        
}manager_t;

typedef class idle:state{
    public:
        idle();
        idle(manager_t *p);
        ~idle();
        void enter();
        void exit();
        bool cond(int code);
        void proc(int code);
    private:
        manager_t * parent;
        
}idle_t;



};