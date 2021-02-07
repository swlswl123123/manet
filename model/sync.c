#include <opnet.h>
#include <sync.h>
#include <stdio.h>
#include <log.h>
// #include <opnet.h>


void ant_set_bearing_angle(ant_t *a,double bear)
{
    a->gamma = bear;
    a->alpha = 0;
    op_ima_obj_attr_set_dbl(a->ant_id, "target longitude", a->gamma -180);
    op_ima_obj_attr_set_dbl(a->ant_id, "target latitude", a->alpha);
}

void ant_set_bearing(ant_t *a, int sector)
{
    a->gamma = (sector - 1.0) * 4.5;
    a->alpha = 0;
    op_ima_obj_attr_set_dbl(a->ant_id, "target longitude", a->gamma -180);
    op_ima_obj_attr_set_dbl(a->ant_id, "target latitude", a->alpha);
}

int on_s_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == S)
    {
        // LOG("S");
        return 1;
    }
    else
        return 0;
}

void on_s_proc(clk_t *clock, int b)
{
    for(int i=1;i<9;i++)
    {
        // LOG("[%d]RX SEC[%d] = %d",clock->id,i,b*8+i);
        ant_set_bearing(&clock->antennas[i],b*8+i);
    }
}

int on_o_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == O)
    {
        // LOG("O");
        return 1;
    }
    else
        return 0;
}

void on_o_proc(clk_t *clock, int b)
{

    if (clock->mode)
    {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 62; j++)
            {
                op_intrpt_schedule_self(op_sim_time() + 2 * i + 0.05 * j + 0.01 * b, C | j);
            }
    }
    else
    {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 62; j++)
            {
                op_intrpt_schedule_self(op_sim_time() + 2 * i + 0.05 * j, C | i);
            }
    }
}

int on_c_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == C)
    {
        // LOG("C");
        return 1;
    }
    else
        return 0;
}

void on_c_proc(clk_t *clock, int b)
{
    if (clock->mode)
    {
        for (int i = 0; i < 12; i++)
        {
            op_intrpt_schedule_self(op_sim_time() + i * 0.00075, N | (12 * b + i));
        }
    }
    else
    {
        // LOG("b = %d",b);
        op_intrpt_schedule_self(op_sim_time() + 0.00005, S | b);
    }
}

int on_nd_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == ND)
    {
        // LOG("ND");
        return 1;
    }
    else
        return 0;
}

void on_nd_proc(clk_t *clock, int b)
{
    if (clock->mode)
    {
        switch (b)
        {
        case ND1_SENT:
        {
            // LOG("[%d]ND1 SEND",clock->id);
            unsigned long long cur_clk = clock->ref_clk+op_sim_time()*1000000000;
            // LOG("[%d]ref_clk = %llu",clock->id,cur_clk);
            Packet *p = op_pk_create(0);
            op_pk_fd_set_int32(p,0,clock->id,32);
            op_pk_fd_set_int32(p,1,clock->ref_id,32);
            op_pk_fd_set_int32(p,2,ND1,32);
            op_pk_fd_set_int64(p,3,cur_clk,64);
            op_pk_fd_set_int32(p,4,0,32);
            op_pk_send(p,ANT_045_DL);
            // op_pk_print(p);
            clock->state = ND1_SENT;
        }
            break;
        case ND2_WAIT:
            // LOG("[%d]ND2_WAIT",clock->id);
            if(clock->p_ant_045_ul!=OPC_NIL)
            {
                unsigned int dist = 0;
                unsigned long long remote_clk;
                unsigned long long ref_clk;
                unsigned long long local_clk = op_sim_time() * 1e9 + clock->ref_clk;
                int ref_id,id;
                op_pk_fd_get_int32(clock->p_ant_045_ul,0,&id);
                op_pk_fd_get_int32(clock->p_ant_045_ul,1,&ref_id);
                op_pk_fd_get_int64(clock->p_ant_045_ul,3,(long long *)&remote_clk);
                // op_pk_fd_get_int32(clock->p_ant_045_ul,4,&dist);
                dist = (clock->align_clk - (clock->ref_clk + op_sim_time()*1e9 - 400e3)-50e-3)/2;
                if(dist > 300000){
                    op_pk_destroy(clock->p_ant_045_ul);
                    clock->p_ant_045_ul = OPC_NIL;
                    clock->current_id = 0;
                    clock->align_clk = 0;
                    return;
                }

                Packet *p = op_pk_create(0);
                op_pk_fd_set_int32(p,0,clock->id,32);
                op_pk_fd_set_int32(p,1,clock->ref_id,32);
                op_pk_fd_set_int32(p,2,2,32);
                op_pk_fd_set_int64(p,3,(long long)(clock->ref_clk+op_sim_time()*1e9),64);
                op_pk_fd_set_int32(p,4,dist,32);
                // op_pk_print(p);
                op_pk_send(p,ANT_045_DL);

                if(ref_id < clock->ref_id)
                {
                    LOG("[%d]<-[%d] remote = %llu local = %llu aligns = %llu dist = %lu",clock->id,id,remote_clk,local_clk,clock->align_clk,dist);
                    clock -> ref_id = ref_id;
                    clock -> ref_clk =  ((remote_clk + dist)  - clock->align_clk) + clock -> ref_clk;
                    // LOG("[%d]CLOCK ALIGN remote = %llu local = %llu",clock->id,remote_clk,clock->ref_clk);
                    LOG("[%d]<-[%d]ref_id = %d ref_clk = %llu",clock->id,id,clock->ref_id,clock->ref_clk);
                }


                op_pk_destroy(clock->p_ant_045_ul);
                clock->p_ant_045_ul = OPC_NIL;
            }
            break;
        case ND3_SENT:
        default:
            break;
        }
    }
    else
    {
        switch (b)
        {

        case ND2_SENT:
        {
            // LOG("[%d]ND2_SENT",clock->id);
            int id,ref_id,phase;
            unsigned long long clk;
            unsigned long long cur_clk = 0;
            op_pk_fd_get_int32(clock->p_ant_400_ul,0,&id);
            op_pk_fd_get_int32(clock->p_ant_400_ul,1,&ref_id);
            op_pk_fd_get_int32(clock->p_ant_400_ul,2,&phase);
            op_pk_fd_get_int64(clock->p_ant_400_ul,3,(long long *)(&clk));
            // op_pk_print(clock->p_ant_400_ul);

            // if(ref_id < clock->ref_id)
            // {
                // LOG("[%d]<-[%d] CLOCK ALIGN",clock->id,ref_id);

            cur_clk = clock->ref_clk + op_sim_time()*1000000000;
            // LOG("[%d]cur_clk_ND2 = %llu",clock->id,cur_clk);
            Packet *p = op_pk_create(0);
            op_pk_fd_set_int32(p,0,clock->id,32);
            op_pk_fd_set_int32(p,1,clock->ref_id,32);
            op_pk_fd_set_int32(p,2,1,32);
            op_pk_fd_set_int64(p,3,cur_clk,64);
            op_pk_fd_set_int32(p,4,0,32);
            ant_set_bearing_angle(&clock->antennas[0],clock->income_bearing);
            op_pk_send(p,ANT_045_DL);
            clock->state = ND2_SENT;
            // } else if (ref_id > clock->ref_id)
            // {
                // LOG("[%d]CLOCK INFORM",clock->id);
                // clock->state = IDLE;
            // } else 
            // {
                // LOG("[%d] CLOCK IGNORE",clock->id);
                // clock->state = IDLE;

            // }
            op_pk_destroy(clock->p_ant_400_ul);
            clock->p_ant_400_ul = OPC_NIL;
        }
            break;
        case ND3_WAIT:
        {
            // LOG("[%d]ND3_WAIT",clock->id);
            clock->state = IDLE;
            if(clock->p_ant_400_ul != OPC_NIL)
            {

                int id,ref_id;
                int dist = 0;
                unsigned long long remote_clock = 0;
                unsigned long long local_clock = clock->ref_clk + op_sim_time() * 1e9;

                // op_pk_print(clock->p_ant_400_ul);

                op_pk_fd_get_int32(clock->p_ant_400_ul,0,&id);
                op_pk_fd_get_int32(clock->p_ant_400_ul,1,&ref_id);
                op_pk_fd_get_int64(clock->p_ant_400_ul,3,(long long *)(&remote_clock));
                op_pk_fd_get_int32(clock->p_ant_400_ul,4,&dist);

                if(ref_id < clock->ref_id)
                {
                    LOG("[%d]<-[%d] remote = %llu local = %llu aligns = %llu dist = %lu",clock->id,id,remote_clock,local_clock,clock->align_clk,dist);
                    clock->ref_id = ref_id;
                    clock->ref_clk = (remote_clock - (clock->align_clk - dist)) + clock->ref_clk;
                    // LOG("[%d]CLOCK ALIGN remote = %llu local = %llu",clock->id,remote_clock,clock->ref_clk);
                    LOG("[%d]<-[%d]ref_id = %d ref_clk = %llu",clock->id,id,clock->ref_id,clock->ref_clk);

                }

                op_pk_destroy(clock->p_ant_400_ul);
                clock->p_ant_400_ul = OPC_NIL;
            }
        }
            break;
        default:
            break;
        }
    }
}

int on_n_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == N)
        return 1;
    else
        return 0;
}

void on_n_proc(clk_t *clock, int b)
{
    if (clock->mode)
    {
        // LOG("[%d] TX sec = %d",clock->id,sec_seq[b]);
        ant_set_bearing(&(clock->antennas[0]),sec_seq[b]);
        op_intrpt_schedule_self(op_sim_time() + 50e-6, ND | ND1_SENT);
        op_intrpt_schedule_self(op_sim_time() + 500e-6, ND | ND2_WAIT);
        op_intrpt_schedule_self(op_sim_time() + 500e-6 + 50e-6, ND | ND3_SENT);
    }
    else
    {
        op_intrpt_schedule_self(op_sim_time() + 50e-6, ND | ND2_SENT);
        op_intrpt_schedule_self(op_sim_time() + 500e-6 - 1e-6, ND | ND3_WAIT);
    }
}

int on_r_cond(int t, int s)
{
    if (t == OPC_INTRPT_SELF && s == R)
    {
        // LOG("R");
        return 1;
    }
    else
        return 0;
}

void on_r_proc(clk_t *clock, int b)
{
    // LOG("ON_RPROC");
    if (b < 2)
    {
        op_intrpt_schedule_self(op_sim_time() + 100, R | (b + 1)); //100秒为一轮
        clock->mode = (mode_t) (op_prg_random_integer_gen(clock->gen) % 2);    //确定本轮扫描模式
        LOG("[%d]MODE = %d ",clock->id, clock->mode);
        if (clock->mode)
        {
            for (int i = 0; i < 5; i++)
            {
                op_intrpt_schedule_self(op_sim_time() + 20 * i, O | i); //安排annoucing节点中断
            }
        }
        else
        {
            for (int i = 0; i < 5; i++)
            {
                op_intrpt_schedule_self(op_sim_time() + 20 * i, O | i); //安排monitoring节点中断
            }
        }
    }
}

int on_rx_400_cond(int t, int s)
{
    if (t == OPC_INTRPT_STRM && (s == ANT_045_0_UL || s == ANT_045_1_UL || s == ANT_045_2_UL || s == ANT_045_3_UL || s == ANT_045_4_UL || s == ANT_045_5_UL || s == ANT_045_6_UL || s == ANT_045_7_UL))
    {
        // LOG("rx_400");
        return 1;
    }
    else
        return 0;
}
void on_rx_400_proc(clk_t *clock, int b)
{
    if (clock->mode)
    {
        op_pk_destroy(op_pk_get(b));
    }
    else
    {
        if(clock->p_ant_400_ul == OPC_NIL){

            switch (clock->state)
            {
            case IDLE:
            {
                int phase = 0;
                int id = 0;
                clock->p_ant_400_ul = op_pk_get(b);
                op_pk_fd_get_int32(clock->p_ant_400_ul,2,&phase);
                if(phase == 0)
                {
                    // LOG("[%d]ND1 RECV",clock->id);
                    op_pk_fd_get_int32(clock->p_ant_400_ul,0,&clock->current_id);
                    op_intrpt_schedule_self(op_sim_time(),N|b);
                    // op_pk_print(clock->p_ant_400_ul);
                    clock->income_bearing = clock->antennas[b - 1].gamma;
                    clock->align_clk = op_sim_time()*1000000000 + clock->ref_clk;
                    // LOG("[%d]<-[%d]ND1",clock->id,clock->current_id);
                } else {
                    op_pk_destroy(clock->p_ant_400_ul);
                    clock->p_ant_400_ul = OPC_NIL;
                }
            }
            break;
            case ND2_SENT:
            {
                // LOG("[%d]ND_SENT",clock->id);
                int phase = 0;
                int id = 0;
                clock->p_ant_400_ul = op_pk_get(b);
                // op_pk_print(clock->p_ant_400_ul);
                op_pk_fd_get_int32(clock->p_ant_400_ul,2,&phase);
                op_pk_fd_get_int32(clock->p_ant_400_ul,0,&id);
                if(phase == 2 && id == clock->current_id)
                {
                    // LOG("[%d]<-[%d]ND3",clock->id,clock->current_id);
                    clock->align_clk = op_sim_time()*1000000000 + clock->ref_clk;

                } else {
                    op_pk_destroy(clock->p_ant_400_ul);
                    clock->p_ant_400_ul = OPC_NIL;
                }
            }
            break;
            default:
            {
                op_pk_destroy(op_pk_get(b));
                clock->p_ant_400_ul  = OPC_NIL;
            }
                break;
            }
        } else
        {
            op_pk_destroy(op_pk_get(b));
        }
        
    }
}

int on_rx_045_cond(int t, int s)
{
    if (t == OPC_INTRPT_STRM && s == ANT_045_UL){
        // LOG("on_rx_045");
        return 1;
    }
    else
        return 0;
}

void on_rx_045_proc(clk_t *clock, int b)
{
    if (clock->mode)
    {
        if (clock->p_ant_045_ul == OPC_NIL)
        {
            switch (clock->state)
            {
            case ND1_SENT:
            {
                int phase = 0;
                clock->p_ant_045_ul = op_pk_get(b);
                op_pk_fd_get_int32(clock->p_ant_045_ul,2,&phase);
                if(phase == 1)
                {
                    op_pk_fd_get_int32(clock->p_ant_045_ul,0,&clock->current_id);
                    clock->align_clk = op_sim_time()*1000000000 + clock->ref_clk;
                    // LOG("[%d]<-[%d]ND2",clock->id,clock->current_id);
                } else {
                    op_pk_destroy(clock->p_ant_045_ul);
                    clock->p_ant_045_ul = OPC_NIL;
                }
            }
            break;
            default:
            {
                op_pk_destroy(op_pk_get(b));
                clock->p_ant_045_ul  = OPC_NIL;
            }
                break;
            }
        }
        else
        {
            op_pk_destroy(op_pk_get(ANT_045_UL));
            
        }
    }
    else
    {
        op_pk_destroy(op_pk_get(ANT_045_UL));
    }
}

void on_endsim(clk_t *clock)
{
    LOG("[%d]ref_id = %d ref_clk = %llu",clock->id,clock->ref_id,clock->ref_clk);
}

void clock_init(clk_t *clock)
{
    Objid obj = op_id_parent(op_id_self());
    char name[256];

    if (op_ima_obj_attr_exists(obj, "user id"))
    {
        op_ima_obj_attr_get(obj, "user id", &(clock->id));
    }
    else
    {
        LOGD("user id not exits!");
    }
    clock->gen = op_prg_random_gen_create(clock->id);
    clock->my_clk = op_prg_random_integer_gen(clock->gen); //随机初始化时钟
    clock->ref_id = clock->id;
    clock->ref_clk = clock->my_clk;
    // LOG("[%d]my_clk = %lu", clock->id, clock->my_clk);
    LOG("[%d]ref_id = %d ref_clk = %llu",clock->id,clock->ref_id,clock->ref_clk);
    clock->p_ant_045_ul = OPC_NIL;
    clock->p_ant_400_ul = OPC_NIL;

    op_intrpt_schedule_self(op_sim_time() + (2000000000 - (clock->my_clk % 2000000000)) / 1e9, R | 0x00); //安排整2秒时开始扫描

    for (int i = 0; i < op_topo_child_count(obj, OPC_OBJTYPE_ANT); i++)
    {
        int arid = 0;
        op_ima_obj_attr_get_str(op_topo_child(obj, OPC_OBJTYPE_ANT, i), "name", 256, name);
        sscanf(name, "ar_%d", &arid);
        clock->antennas[arid].ant_id = op_topo_child(obj, OPC_OBJTYPE_ANT, i);

        // LOG("[%d]%s objid = %d", clock->id, name, clock->antennas[arid].ant_id);
    }
}