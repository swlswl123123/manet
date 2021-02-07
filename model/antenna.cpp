#include "antenna.hpp"
// #include <opnet.h>
// #include <vos.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

antenna::antenna()
{
    node = op_id_parent(op_id_self());
    op_ima_obj_attr_get_int32(node, "user id", &id);
    LOGD("[%d]antenna", id);
}
antenna::~antenna() {}
geo_t antenna::calc_pointing(geo_t p, angle_t t)
{
    double gamma = t.gamma / 180 * M_PI;
    double alpha = t.alpha / 180 * M_PI;
    double D = t.D;
    double lambda = p.lon / 180 * M_PI;
    double phi = p.lat / 180 * M_PI;
    double h = p.alt;

    double y1 = 0;
    double y2 = 0;
    double y3 = 0;

    y1 = cos(lambda) * cos(phi) * (RE + h) + D * cos(lambda) * cos(phi) * sin(alpha) - D * cos(alpha) * sin(gamma) * sin(lambda) - D * cos(alpha) * cos(gamma) * cos(lambda) * sin(phi);
    y2 = cos(phi) * sin(lambda) * (RE + h) + D * cos(alpha) * cos(lambda) * sin(gamma) + D * cos(phi) * sin(alpha) * sin(lambda) - D * cos(alpha) * cos(gamma) * sin(lambda) * sin(phi);
    y3 = sin(phi) * (RE + h) + D * sin(alpha) * sin(phi) + D * cos(alpha) * cos(gamma) * cos(phi);
    double y = sqrt(y1 * y1 + y2 * y2 + y3 * y3);

    geo_t ret;
    double alt = y - RE;
    double lat = asin(y3 / y);
    double lon = 0;

    if (y1 == 0 && y2 == 0)
    {
        lon = 0;
    }
    else
    {
        if (y2 > 0)
        {
            lon = acos(y1 / (y * cos(lat)));
        }
        else
        {
            lon = -acos(y1 / (y * cos(lat)));
        }
    }

    ret.lat = lat / M_PI * 180;
    ret.lon = lon / M_PI * 180;
    ret.alt = alt;
    // printf("y = (%lf,%lf,%lf)\n",y1,y2,y3);
    // printf("geo = (%lf,%lf,%lf)\n",ret.lat,ret.lon,ret.alt);
    return ret;
}

void antenna::execute_pointing(angle_t t)
{
    if (op_ima_obj_attr_exists(node, "ar.target altitude") && op_ima_obj_attr_exists(node, "ar.target latitude") && op_ima_obj_attr_exists(node, "ar.target longitude"))
    {

        //坐标系换算部分计算移动至dra_[ra/ta]gain.ps.c
        /*
            geo_t pos;
            op_ima_obj_attr_get_dbl(node,"x position",&pos.lon);
            op_ima_obj_attr_get_dbl(node,"y position",&pos.lat);
            op_ima_obj_attr_get_dbl(node,"altitude",&pos.alt);

            geo_t pointing = calc_pointing(pos,t);
            op_ima_obj_attr_set_dbl(node,"ar.target altitude",pointing.alt);
            op_ima_obj_attr_set_dbl(node,"ar.target latitude",pointing.lat);
            op_ima_obj_attr_set_dbl(node,"ar.target longitude",pointing.lon);

            // //获取天线参数以确认配置生效
            // op_ima_obj_attr_get_dbl(node,"ar.target altitude",&pointing.alt);
            // op_ima_obj_attr_get_dbl(node,"ar.target latitude",&pointing.lat);
            // op_ima_obj_attr_get_dbl(node,"ar.target longitude",&pointing.lon);
            
            // LOGD("[%d](lon = %f, lat = %f, h = %f gamma = %f,alpha = %f)->(lon = %f,lat = %f, alt = %f)",id,pos.lon,pos.lat,pos.alt,t.gamma,t.alpha,
            // pointing.lon,pointing.lat,pointing.alt);
        */

        op_ima_obj_attr_set_dbl(node, "ar.target latitude", t.alpha);
        op_ima_obj_attr_set_dbl(node, "ar.target longitude", t.gamma - 180);
        LOGD("[%d]scan@%f deg", id, t.gamma);
        pointing.alpha = t.alpha;
        pointing.gamma = t.gamma;
        pointing.D = t.D;
    }
    else
    {
        LOGD("attributes not exits!!");
    }
}