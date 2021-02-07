#include <r.hpp>

r::r(){};
r::r(dtdma_t *proc):parent(proc),r_cnt(0){};
r::~r(){};
bool r::cond(int cond){
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    return (type == OPC_INTRPT_SELF && code == cond);
};
void r::proc(int cond){
    switch (cond){
    case R1:
        LOGD("[%d]proc:R1",parent->id);
        op_intrpt_schedule_self(op_sim_time()+BEACON_R1,R2);
        break;

    case R2:
        LOGD("[%d]proc:R2",parent->id);
        op_intrpt_schedule_self(op_sim_time()+BEACON_R2,R3);
        break;
    case R3:
        LOGD("[%d]proc:R3",parent->id);
        r_cnt++;
        if(r_cnt>=NUM_R_PER_SHORT_FRAME){
            op_intrpt_schedule_self(op_sim_time(),D0);
        } else {
            op_intrpt_schedule_self(op_sim_time(),R1);
        }
        break;
    default:
        break;
    }

};
void r::enter(){
    int type = op_intrpt_type();
    int code = op_intrpt_code();
    if(type == OPC_INTRPT_SELF && code == R0){
        op_intrpt_schedule_self(op_sim_time(),R1);
    }
};
void r::exit(){};