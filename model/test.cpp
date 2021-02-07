// #include "antenna.hpp"
// #include <math.h>
#include <stdio.h>
#include <packet.hpp>
typedef struct {
      unsigned long long slot:40;//空闲时隙向量
}bov_t;

typedef struct{
    unsigned long long clock_ref:48;  //时钟参考节点id(以太网)
    unsigned long long time_ref:64;   //时钟参考节点时间戳
}sync_t;

typedef struct{
  unsigned long long src:48;  //源节点地址(以太网)
  unsigned long long dst:48;  //目的节点地址(以太网)
  unsigned long long time; //时间戳
  bool ext_clock:1; //是否有外部时统
  bool is_synced:1; //是否已同步
  bool is_innet:1;  //是否已入网
  union{
    sync_t sync;
    bov_t bov;
  };

  unsigned char var[0];
}nd_1;



int main(int argc,char *argv[]){

  printf("%ld",sizeof(nd_t));
}