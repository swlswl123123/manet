#pragma once
#include <state.hpp>
#include <dtdma.hpp>

typedef class nd:state{
    private:
        dtdma_t * parent;
        int scan_cnt;
        int scan_dir;
        int scan_sector;
    public:
        nd(dtdma_t *proc);
        ~nd();
        bool cond(int cond);
        void proc(int cond);
        void enter();
        void exit();
        typedef enum cond{
            IDLE,LOOP,R
        }cond_t;
}nd_t;