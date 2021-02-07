#pragma once

#define PI 3.1415926535

typedef struct pos
{
  unsigned long long clk;
  int x;
  int y;
  int z;
  int v_x;
  int v_y;
  int v_z;
}pos_t;

typedef struct dpos
{
  double x;
  double y;
  double z;
}dpos_t;

typedef class position_manager
{
private:
    pos_t pos_table[20];
public:
    position_manager(/* args */);
    ~position_manager();
    int modify_beam_angle(int d, void *parent);
    void get_position_send(void *parent);
    void set_position_recv(Packet* r);
}position_manager_t;
