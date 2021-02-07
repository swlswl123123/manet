#pragma once
#include <packet.hpp>
#include <router.hpp>
#include <balloc.hpp>
#include<opnet.h>
typedef class node node_t;
class node : public vertex
{

private:
    balloc::beacon_info_t bov;
public:
    unsigned long long last_update; //last update time
    node();
    node(int id);
    node(const balloc::beacon_info_t & info,unsigned long long time = 0);
    ~node();
    bool operator==(const node_t &n) const;
    bool operator>(const node_t &n) const;
    bool operator<(const node_t &n) const;
    balloc::beacon_info_t get_bov()const;

    int get_id() const;
    int operator|(const node_t &b) const;
    int operator||(const node_t &b) const;
    void update(balloc::beacon_info_t &info,unsigned long long time = 0);
};
