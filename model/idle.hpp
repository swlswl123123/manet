#pragma once
#include <state.hpp>
#include <dtdma.hpp>
#include <position_manager.hpp>

typedef class idle : state
{
private:
    dtdma_t *parent;
    bool isstar;
    friend class position_manager;
    int d_consecutive;
    int d_cnt;
    bool d_flag;
public:
    idle();
    idle(dtdma_t *proc);
    ~idle();
    bool cond(int cond);
    bool cond();
    void proc(int cond);
    void proc();
    void enter();
    void exit();
    void print_Q();
    void print_beacon_table();
    void print_forward_table();
} idle_t;