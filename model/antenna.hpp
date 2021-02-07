#pragma once
#include <opnet.h>
#define RE 6378137.0

typedef struct
{
    double gamma;
    double alpha;
    double D;
} angle_t;

typedef struct
{
    double lat;
    double lon;
    double alt;
} geo_t;

typedef class antenna antenna_t;
class antenna
{

private:
    int id;
    Objid node, ant;

public:
    angle_t pointing;
    static geo_t calc_pointing(geo_t p, angle_t t);
    antenna();
    ~antenna();
    void execute_pointing(angle_t t);
};
