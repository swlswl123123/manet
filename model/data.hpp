#pragma once
#include <state.hpp>
#include <dtdma.hpp>
typedef class data:state{
    private:
        dtdma_t * parent;
        int d_cnt;
    public:
        data();
        ~data();
        data(dtdma_t * proc);
        bool cond(int cond);
        void proc(int cond);
        void enter();
        void exit();
        typedef enum cond{R,LOOP,IDLE}cond_t;
}data_t;