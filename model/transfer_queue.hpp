#pragma once
#include <queue>
#include <vector>
#include <algorithm>
#include <packet.hpp>

using namespace std;
class transfer_queue:public priority_queue<pri_packet_t>
{
private:
    unsigned long buffer_size;
public:
    transfer_queue(/* args */);
    ~transfer_queue();
    void push(const pri_packet_t &p);
    void pop();
    unsigned long calc_size()const;
};