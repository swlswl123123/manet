#pragma once
#include <state.hpp>
#include <dtdma.hpp>

typedef class r:state{
    private:
        dtdma_t * parent;
        int r_cnt;
    public:
        r();
        r(dtdma_t *proc);
        ~r();
        bool cond(int cond);
        void proc(int cond);
        void enter();
        void exit();
        typedef enum cond{ND,LOOP,DATA}cond_t;
}r_t;