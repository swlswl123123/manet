#pragma once

const unsigned int sec_seq[] = {

     1, 2, 3, 4,17,18,19,20,21,22,23,24,41,42,43,44,
    25,26,27,28,29,30,31,32, 5, 6, 7, 8,33,34,35,36,
    37,38,39,40,45,46,47,48,53,54,55,56,57,58,59,60,
     9,10,11,12,61,62,63,64,65,66,67,68,49,50,51,52,
    69,70,71,72,73,74,75,76,13,14,15,16,77,78,79,80,
     1, 2, 3, 4,53,54,55,56, 5, 6, 7, 8, 9,10,11,12,
    17,18,19,20,13,14,15,16,29,30,31,32,57,58,59,60,
    33,34,35,36,37,38,39,40,21,22,23,24,41,42,43,44,
    45,46,47,48,61,62,63,64,49,50,51,52,69,70,71,72,
    25,26,27,28,73,74,75,76,77,78,79,80,65,66,67,68,
     1, 2, 3, 4, 5, 6, 7, 8,29,30,31,32, 9,10,11,12,
    13,14,15,16,69,70,71,72,17,18,19,20,21,22,23,24,
    33,34,35,36,25,26,27,28,41,42,43,44,73,74,75,76,
    45,46,47,48,49,50,51,52,37,38,39,40,53,54,55,56,
    57,58,59,60,77,78,79,61,62,63,64,65,66,67,68,80,
    41,42,43,44,13,14,15,16,17,18,19,20, 1, 2, 3, 4,
    21,22,23,24,25,26,27,28,45,46,47,48,29,30,31,32,
    33,34,35,36, 5, 6, 7, 8,37,38,39,40,57,58,59,60,
    49,50,51,52,61,62,63,64,65,66,67,68, 9,10,11,12,
    69,70,71,72,73,74,75,76,53,54,55,56,77,78,79,80,
     1, 2, 3, 4,13,14,15,16, 5, 6, 7, 8, 9,10,11,12,
    57,58,59,60,29,30,31,32,33,34,35,36,17,18,19,20,
    37,38,39,40,41,42,43,44,61,62,63,64,45,46,47,48,
    49,50,51,52,21,22,23,24,53,54,55,56,69,70,71,72,
    65,66,67,68,73,74,75,76,77,78,79,80,25,26,27,28,
     1, 2, 3, 4, 5, 6, 7, 8,69,70,71,72, 9,10,11,12,
    13,14,15,16,29,30,31,32,17,18,19,20,21,22,23,24,
    73,74,75,76,25,26,27,28,41,42,43,44,33,34,35,36,
    45,46,47,48,49,50,51,52,77,78,79,80,53,54,55,56,
    57,58,59,60,37,38,39,40,61,62,63,64,65,66,67,68

};


typedef struct ant
{
    Objid ant_id;
    double alpha;
    double gamma;
}ant_t;

typedef enum code{
    N = 0x00001000,
    ND = 0x00002000,
    R  = 0x00003000,
    C = 0x00004000,
    S = 0x00005000,
    O = 0x00006000,
    CIRC = 0x00007000
}code_t;

typedef enum strm
{
    ANT_045_UL,     //4.5度窄带天线接收
    ANT_045_DL,     //4.5度窄带天线发射
    ANT_045_0_UL,      //36度宽带天线接收
    ANT_045_1_UL,      //36度宽带天线接收
    ANT_045_2_UL,      //36度宽带天线接收
    ANT_045_3_UL,      //36度宽带天线接收
    ANT_045_4_UL,      //36度宽带天线接收
    ANT_045_5_UL,      //36度宽带天线接收
    ANT_045_6_UL,      //36度宽带天线接收
    ANT_045_7_UL      //36度宽带天线接收
}strm_t;

typedef enum mode
{
    MONITORING,
    ANNOUNCING
}mode_t;

typedef enum state
{
    IDLE,
    ND1_SENT,
    ND1_WAIT,
    ND2_SENT,
    ND2_WAIT,
    ND3_SENT,
    ND3_WAIT
}state_t;

typedef enum phase
{
    ND1,
    ND2,
    ND3
}phase_t;

typedef struct clk clk_t;

typedef int (*cond_t)(int,int);
typedef void (*proc_t)(clk_t *,int);

typedef struct intrpt
{
    cond_t cond;
    proc_t proc;
}intrpt_t;

struct clk
{
    int id;
    int ref_id;
    unsigned long long my_clk;
    unsigned long long ref_clk;
    unsigned int r_cnt;
    unsigned int n_cnt;
    unsigned int s_cnt;
    mode_t mode;
    state_t state;
    PrgT_Random_Gen * gen;
    Packet * p_ant_400_ul;
    Packet * p_ant_045_ul;
    int current_id;
    ant_t antennas[9];
    double income_bearing;
    unsigned long long align_clk;
};
