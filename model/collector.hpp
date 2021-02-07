#pragma once
#include <state.hpp>

namespace collector_ns
{

enum stat
{
    DELAY_UPDATE,
    URG_DELAY_UPDATE,
    BIT_RATE_UPDATE
};

enum intrpt
{
    ON_DELAY_UPDATE,
    ON_URG_DELAY_UPDATE,
    ON_BIT_RATE_UPDATE
};

class collector : state
{
public:
    collector();
    ~collector();
    void enter();
    void exit();
    bool cond(int code);
    void proc(int code);

private:
    int id;
};

}; // namespace collector_ns