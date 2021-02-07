#pragma once
#include <state.hpp>
#include <vector>

namespace app_ns
{

typedef enum strm
{
    DATA_UL,
    DATA_DL,
    DATA_URG_UL,
    DATA_URG_DL
} strm_t;

typedef enum intrpt
{
    ON_RECV,
    ON_SEND,
    ON_BEGSIM,
    ON_ENDSIM
} intrpt_t;

typedef enum code
{
    SEND = 0x80000000
} code_t;

typedef class traffic traffic_t;
class traffic
{
public:
    int id;
    int src;
    int dst;
    double time;
    int size;
    traffic();
    ~traffic();
    int urg;
    bool operator==(const traffic_t &t) const;
    bool operator>(const traffic_t &t) const;
    bool operator<(const traffic_t &t) const;
};

typedef class app : public state
{

private:
private:
    std::vector<traffic> traffics;

public:
    bool cond(int cond);
    void proc(int proc);

} app_t;

} // namespace app_ns
