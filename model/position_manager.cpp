#include <position_manager.hpp>
#include <dtdma.hpp>

position_manager::position_manager(/* args */)
{
    memset(pos_table, 0x00, sizeof(pos_table));
}

position_manager::~position_manager()
{
}

// dpos_t EarthToRect(double x, double y, double z)
// {
// 	dpos_t re;
// 	double phi = 30.0/180*PI;
// 	double lambda = 120.0/180*PI;
// 	double g_x = (RE)*cos(phi)*cos(lambda);
// 	double g_y = (RE)*cos(phi)*sin(lambda);
// 	double g_z = (RE)*sin(phi);
// 	re.x = -sin(lambda)*(x-g_x) + cos(lambda)*(y-g_y);
// 	re.y = -cos(lambda)*sin(phi)*(x-g_x) - sin(lambda)*sin(phi)*(y-g_y) + cos(phi)*(z-g_z);
// 	re.z = cos(lambda)*cos(phi)*(x-g_x) + cos(phi)*sin(lambda)*(y-g_y) + sin(phi)*(z-g_z);
// 	return re;
// }

dpos_t EarthToRect(double dx, double dy, double dz, double x0, double y0, double z0)
{
	dpos_t re;
	double phi = atan(z0 / sqrt(x0 * x0 + y0 * y0));
	double lambda = acos(x0 / y0);
    // LOG("altitude:%f longitude:%f", phi/PI*180, lambda/PI*180);
	re.x = -sin(lambda)*(dx) + cos(lambda)*(dy);
	re.y = -cos(lambda)*sin(phi)*(dx) - sin(lambda)*sin(phi)*(dy) + cos(phi)*(dz);
	re.z = cos(lambda)*cos(phi)*(dx) + cos(phi)*sin(lambda)*(dy) + sin(phi)*(dz);
	return re;
}

int position_manager::modify_beam_angle(int d, void * ptr)
{
    dtdma_t* parent = (dtdma_t *) ptr;


	if(parent->beacon_table[d].id == 0 || pos_table[parent->beacon_table[d].id - 1].clk == 0)
	{
		return -1;
	}
	double time = (double)pos_table[parent->beacon_table[d].id - 1].clk / 1e9;
	double earth_x = pos_table[parent->beacon_table[d].id - 1].x / 100.0 + (double)pos_table[parent->beacon_table[d].id - 1].v_x / 100 * (op_sim_time() - time);
	double earth_y = pos_table[parent->beacon_table[d].id - 1].y / 100.0 + (double)pos_table[parent->beacon_table[d].id - 1].v_y / 100 * (op_sim_time() - time);
	double earth_z = pos_table[parent->beacon_table[d].id - 1].z / 100.0 + (double)pos_table[parent->beacon_table[d].id - 1].v_z / 100 * (op_sim_time() - time);
	double self_time = (double)pos_table[parent->id - 1].clk / 1e9;
	double earth_self_x = pos_table[parent->id - 1].x /100.0 + (double)pos_table[parent->id - 1].v_x / 100 * (op_sim_time() - self_time);
	double earth_self_y = pos_table[parent->id - 1].y /100.0 + (double)pos_table[parent->id - 1].v_y / 100 * (op_sim_time() - self_time);
	double earth_self_z = pos_table[parent->id - 1].z /100.0 + (double)pos_table[parent->id - 1].v_z / 100 * (op_sim_time() - self_time);
	double distance = sqrt((earth_x - earth_self_x)*(earth_x - earth_self_x) + (earth_y - earth_self_y)*(earth_y - earth_self_y) + (earth_z - earth_self_z)*(earth_z - earth_self_z));
	// auto rect_self = EarthToRect(earth_self_x, earth_self_y, earth_self_z);
	// auto rect_n = EarthToRect(earth_x, earth_y, earth_z);
    auto rect_pos = EarthToRect(earth_x - earth_self_x, earth_y - earth_self_y, earth_z - earth_self_z, earth_self_x, earth_self_y, earth_self_z);
	double dx = rect_pos.x;
	double dy = rect_pos.y;
    double dz = rect_pos.z;
    // LOG("[%d][%d] dx:%f dy:%f dz:%f", parent->id, parent->beacon_table[d].id, dx, dy, dz);
	// LOG("self[%d] earth_x:%f earth_y:%f neighbor[%d] earth_x:%f earth_y:%f", id, earth_self_x, earth_self_y, beacon_table[d].id, earth_x, earth_y);
	// LOG("self[%d] rect_x:%f rect_y:%f neighbor[%d] rect_x:%f rect_y:%f", id, rect_self.x/30e3, rect_self.y/30e3, beacon_table[d].id, rect_n.x/30e3, rect_n.y/30e3);
	double modify_gamma;
    double modify_alpha;
    double r = sqrt(dx*dx + dy*dy);
	if(dx == 0 && dy > 0)
	{
		modify_gamma = 0;
	}
	else if(dx == 0 && dy < 0)
	{
		modify_gamma = 180;
	}
	else if(dx > 0)
	{
		modify_gamma = 90 - atan(dy/dx)/PI*180;
	}
	else if(dx < 0)
	{
		modify_gamma = 270 - atan(dy/dx)/PI*180;
	}

    if (dz == 0)
    {
        modify_alpha = 0;
    }
    else
    {
        modify_alpha = atan(dz/r)/PI*180;
    }
    
    angle_t modify_angle;
    modify_angle.alpha = modify_alpha;
    modify_angle.gamma = modify_gamma;
    modify_angle.D = distance;
	LOG("[%d]modify[%d]: modify_alpha %f modify_gamma %f orign_alpha %f orign_gamma %f distance %f", parent->id, parent->beacon_table[d].id, modify_angle.alpha, modify_angle.gamma, parent->beam_seq[parent->beacon_table[d].id - 1].alpha, parent->beam_seq[parent->beacon_table[d].id - 1].gamma, distance);
    parent->beam_seq[parent->beacon_table[d].id - 1] = modify_angle;
	// if(abs(parent->beam_seq[parent->beacon_table[d].id - 1].alpha - modify_angle.alpha) > 5.625 || abs(parent->beam_seq[parent->beacon_table[d].id - 1].gamma - modify_angle.gamma) > 5.625)
	// {
    //     parent->beam_seq[parent->beacon_table[d].id - 1] = modify_angle;
	// 	LOG("done modify");
	// }
	if(distance > 150e3)
	{
		LOG("[%d]departure from[%d]:%f", parent->beacon_table[d].id, parent->id, distance);
        memset(&pos_table[parent->beacon_table[d].id-1], 0x00, sizeof(pos_t));
		return 1;
	}
	else
	{
		return 0;
	}    
}

void position_manager::get_position_send(void * ptr)
{
    dtdma_t* parent = (dtdma_t *) ptr;
    double lat_ptr, long_ptr, alt_ptr, x_ptr, y_ptr, z_ptr;
    double lat_ptr1, long_ptr1, alt_ptr1, x_ptr1, y_ptr1, z_ptr1;
    double lat_ptr2, long_ptr2, alt_ptr2, x_ptr2, y_ptr2, z_ptr2;
    op_ima_obj_pos_get_time(parent->obj, op_sim_time(), &lat_ptr, &long_ptr, &alt_ptr, &x_ptr, &y_ptr, &z_ptr);
    op_ima_obj_pos_get_time(parent->obj, op_sim_time() + 1, &lat_ptr1, &long_ptr1, &alt_ptr1, &x_ptr1, &y_ptr1, &z_ptr1);
    op_ima_obj_pos_get_time(parent->obj, op_sim_time() + 2, &lat_ptr2, &long_ptr2, &alt_ptr2, &x_ptr2, &y_ptr2, &z_ptr2);

    int x = x_ptr * 1e2;
    int y = y_ptr * 1e2;
    int z = z_ptr * 1e2;

    int dif_x = (x_ptr1 - x_ptr) * 1e2;
    int dif_y = (y_ptr1 - y_ptr) * 1e2;
    int dif_z = (z_ptr1 - z_ptr) * 1e2;

    unsigned long long cur_clk = op_sim_time()*1e9;
    if(cur_clk > pos_table[parent->id - 1].clk)
    {
        pos_table[parent->id - 1].clk = cur_clk;
        pos_table[parent->id - 1].x = x;
        pos_table[parent->id - 1].y = y;
        pos_table[parent->id - 1].z = z;
        pos_table[parent->id - 1].v_x = dif_x;
        pos_table[parent->id - 1].v_y = dif_y;
        pos_table[parent->id - 1].v_z = dif_z;
        LOG("[%d]x_pos:%d, y_pos:%d, z_pos:%d, vx:%d, vy:%d, vz:%d", parent->id, pos_table[parent->id-1].x, pos_table[parent->id-1].y, pos_table[parent->id-1].z, pos_table[parent->id-1].v_x, pos_table[parent->id-1].v_y, pos_table[parent->id-1].v_z);    
    }

    for (int i = 0; i < 20; i++)
    {
        if(parent->forward_table[i].PRIMARY)
        {
            int dst = i + 1;
            LOG("[%d]->[%d]send position", parent->id, dst);
            Packet *p = op_pk_create(0);
            op_pk_fd_set_int32(p, 0, dst, 8);
            op_pk_fd_set_int32(p, 1, parent->id, 8);
            op_pk_fd_set_int32(p, 2, x, 32);
            op_pk_fd_set_int32(p, 3, y, 32);
            op_pk_fd_set_int32(p, 4, z, 32);
            op_pk_fd_set_int32(p, 5, dif_x, 32);
            op_pk_fd_set_int32(p, 6, dif_y, 32);
            op_pk_fd_set_int32(p, 7, dif_z, 32);
            op_pk_fd_set_int64(p, 8, cur_clk, 64);
            Packet *r = parent->encap_mac(p, dst, dtdma_ns::UCBCH, dtdma_ns::EHTERTYPE_POS);
            parent->proc_unicast(r);
        }
    }
}

void position_manager::set_position_recv(Packet* r)
{
    int src_id;
    op_pk_fd_get_int32(r, 1, &src_id);
    op_pk_fd_get_int32(r, 2, &pos_table[src_id - 1].x);
    op_pk_fd_get_int32(r, 3, &pos_table[src_id - 1].y);
    op_pk_fd_get_int32(r, 4, &pos_table[src_id - 1].z);
    op_pk_fd_get_int32(r, 5, &pos_table[src_id - 1].v_x);
    op_pk_fd_get_int32(r, 6, &pos_table[src_id - 1].v_y);
    op_pk_fd_get_int32(r, 7, &pos_table[src_id - 1].v_z);    
    op_pk_fd_get_int64(r, 8, (long long *)(&pos_table[src_id - 1].clk));
    op_pk_destroy(r);
    LOG("recv[%d]x_pos:%d, y_pos:%d, z_pos:%d, vx:%d, vy:%d, vz:%d", src_id, pos_table[src_id-1].x, pos_table[src_id-1].y, pos_table[src_id-1].z, pos_table[src_id-1].v_x, pos_table[src_id-1].v_y, pos_table[src_id-1].v_z);    

}