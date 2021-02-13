#include <dtdma.hpp>
#include <opnet.h>
#include <set>
#include <stack>
#include <vector>
#include <log.h>
#include <log_out.hpp>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace dtdma_ns;

dtdma::dtdma() : scan_count(0), dt_count(0), my_seq_num(0)
{
	pos_mngr = new position_manager();
	//inital id
	int i = 0,j=0;
	obj = op_id_parent(op_id_self());
	if (op_ima_obj_attr_exists(obj, "user id"))
	{
		op_ima_obj_attr_get(obj, "user id", &id);
	}
	else
	{
		LOGD("user id not exits!");
	}

	this->self = new node(id);
	status = INNET;
	memset(beam_seq, 0x00, sizeof(beam_seq));
	memset(beacon_table, 0x00, sizeof(beacon_table));
	memset(forward_table, 0x00, sizeof(forward_table));
	memset(seq_num, 0x00, sizeof(seq_num));

	for (i = 0; i < 10; i++)
        for (j = 0; j < 20; j++)
        {
            scan_seq[0][i * 20 + j].alpha = i < 5 ? -18 / 2.0 - i * 18 : 18 / 2.0 + (i - 5) * 18;
            scan_seq[0][i * 20 + j].gamma = 18 / 2.0 + j * 18;
            scan_seq[1][i * 20 + j].alpha = -scan_seq[0][i * 20 + j].alpha;
            scan_seq[1][i * 20 + j].gamma = (scan_seq[0][i * 20 + j].gamma + 180) > 360 ? (scan_seq[0][i * 20 + j].gamma + 180) - 360 : (scan_seq[0][i * 20 + j].gamma + 180);
        }

	// set scan sequence
	for (int i = 0; i < 50; i++)
	{
		vector<vector<int> > tmp(4, vector<int>(50, 0));
		int num = 1;
		// set main scan seq
		for (int k = 0; k < 4; k++)
		{
			tmp[k][0] = 4*i+k+1;
		}

		// set sub scan seq
		for (int m = 0; m < 4; m++)
		{
			for (int n = 0; n < 50; n++)
			{
				if (tmp[m][n] == 0)
				{
					if(num == 4*i+1)
					{
						num = 4*i+5;
					}
					tmp[m][n] = num++;
				}
			}
		}

		// add to vector
		for (int k = 0; k < 4; k++)
		{
			sec_seq.insert(sec_seq.end(), tmp[k].begin(), tmp[k].end());
		}
	}

};
dtdma::~dtdma(){
	delete pos_mngr;
};

void dtdma::update_forward_table(Packet *p)
{

	if (p == OPC_NIL)
		return;

	for (int i = 0; i < 48; i++)
	{
		unsigned int link = 0;
		op_pk_fd_get_int32(p, i, (int *)&link);
		forward_table[i].PRIMARY = (link >> 7) & 0x00000001;
		forward_table[i].MPR = (link >> 6) & 0x00000001;
		forward_table[i].id = link & 0x0000003f;
		if (forward_table[i].id != 0)
		{
			LOGD("[%d]->[%d]->[%d] %s", id, i + 1, forward_table[i].id, forward_table[i].MPR ? "MPR" : "");
		}
	}
	op_pk_destroy(p);
}

void dtdma::send_proc(int d, int dc)
{
	//Sending
	if (beacon_table[d].mode == balloc::SEND && beacon_table[d].type != balloc::PREORDAINED && beacon_table[d].type != balloc::VACCANT)
	{
		Packet *p = op_pk_create(0);
		unsigned int count = 32;
		int i = 1;
		op_pk_fd_set_int32(p, 0, 0, 32);

		int qid = beacon_table[d].id - 1;
		LOGD("[%d]txQ[%d] size = %d ", parent->id, qid, parent->txQ[qid].size());
		while (txQ[qid].size() > 0)
		{
			Packet *q = (Packet *)txQ[qid].top().get_packet();

			int src, dst, type;
			op_pk_nfd_get_int32(q, "src", &src);
			op_pk_nfd_get_int32(q, "dst", &dst);
			op_pk_nfd_get_int32(q, "type", &type);

			if (type == ETHERTYPE_IP)
			{
				Packet *r = OPC_NIL;
				int pkid = 0;
				op_pk_nfd_get_pkt(q, "data", &r);
				op_pk_fd_get_int32(r, 0, &pkid);
				LOG("[%d]TX[%d]:[%d]->[%d] via [%d]", id, pkid, src, dst, qid + 1);
				op_pk_nfd_set_pkt(q, "data", r);
			}

			if ((count += op_pk_total_size_get(q)) > BIT_PER_UNIT*dc)
				break;
			op_pk_fd_set_pkt(p, i, q, op_pk_total_size_get(q));
			txQ[qid].pop();
			i++;
		}
		if (i > 1)
		{
			op_pk_fd_set_int32(p, 0, i - 1, 32);
			LOG("[%d]TX size = %ld", id, op_pk_total_size_get(p));
			op_pk_send(p, TO_TCB);
		}
		else
		{
			op_pk_destroy(p);
		}
	}
}

void dtdma::recv_proc(Packet *p)
{
	int len, i, src, dst, type, prio;
	Packet *q = OPC_NIL, *r = OPC_NIL, *s = OPC_NIL;
	char fmt[256] = {0};
	if (p != OPC_NIL)
	{
		op_pk_fd_get_int32(p, 0, &len);
		for (i = 0; i < len; i++)
		{
			op_pk_fd_get_pkt(p, i + 1, &q);
			if (q == OPC_NIL)
			{
				LOG("[%d]recv_proc:NO PACKET");
				continue;
			}
			op_pk_format_info_get(q, OPC_PK_PROPERTY_FORMAT_NAME, fmt);
			if (!strstr(fmt, "724mac"))
			{
				LOG("[%d]recv_proc:PACKET_NOT MAC");
				op_pk_destroy(q);
				continue;
			}

			op_pk_nfd_get_int32(q, "dst", &dst);
			op_pk_nfd_get_int32(q, "src", &src);
			op_pk_nfd_get_int32(q, "type", &type);
			op_pk_nfd_get_int32(q, "prio", &prio);

			if (dst == 0x3f)
			{
				proc_broadcast(q);
			}
			else
			{
				proc_unicast(q);
			}

			if (dst == 0x3f)
			{
				op_pk_nfd_get(q, "data", &s);
				if (s != OPC_NIL)
				{
					op_pk_nfd_get(s, "DATA", &r);
				}
				else
				{
					r = OPC_NIL;
				}
			}
			else
			{
				if (dst == id)
				{
					op_pk_nfd_get(q, "data", &r);
				}
				else
				{
					r = OPC_NIL;
				}
			}

			if (r == OPC_NIL)
			{
				op_pk_destroy(q);
				continue;
			}

			switch (type)
			{
			case EHTERTYPE_BALLOC:
				op_pk_send(r, RRC_BALLOC);
				op_pk_destroy(q); // destroy unicast mac header, mine
				break;
			case EHTERTYPE_RINFO:
				LOG("debug_recv_broadcast");
				op_pk_send(r, RRC_LINK_BROADCAST);
				op_pk_destroy(q); // destroy mac header
				op_pk_destroy(s); // destroy broadcast header
				break;
			case EHTERTYPE_POS:
			{
                LOG("[%d]recv pos",id);
				pos_mngr->set_position_recv(r);
				op_pk_destroy(q); // destroy mac header
			}
			break;
			case ETHERTYPE_IP:
			{
				op_pk_send(r, TRAFFIC);
				op_pk_destroy(q);
			}
			break;
			default:
				op_pk_nfd_set_pkt(q, "data", r);
				op_pk_destroy(q);
				break;
			}
		}
		op_pk_destroy(p);
	}
}

void dtdma::proc_broadcast(Packet *p)
{

	int src, dst, type, htl, ctrl, prev, prio;
	unsigned long long seq;
	Packet *q = OPC_NIL, *r = OPC_NIL, *s = OPC_NIL;
	if (p == OPC_NIL)
		return;
	op_pk_nfd_get_int32(p, "dst", &dst);
	op_pk_nfd_get_int32(p, "src", &src);
	op_pk_nfd_get_int32(p, "prio", &prio);
	op_pk_nfd_get_int32(p, "type", &type);
	op_pk_nfd_get_pkt(p, "data", &q);
	op_pk_nfd_get_int32(q, "CTRL", &ctrl);
	op_pk_nfd_get_int32(q, "PREV", &prev);
	op_pk_nfd_get_int32(q, "HTL", &htl);
	op_pk_nfd_get_int64(q, "SEQ", (long long *)&seq);

	LOG("[%d]proc_broadcast:dst = %d src = %d prev = %d type = %d ctrl = %d htl = %d seq = 0x%lx", id, dst, src, prev, type, ctrl, htl, seq);
	if (id == 2)
	{
		for (int i = 0; i < 4; i++)
		{
			LOG("[2]forward_table[%d]:%d", i, forward_table[i].PRIMARY);
		}
	}
	
	if (htl == 0 || seq <= seq_num[src - 1])
	{
		op_pk_nfd_set_pkt(p, "data", q);
		return;
	}

	seq_num[src - 1] = seq;

	op_pk_nfd_set_int32(q, "HTL", htl - 1);

	for (int i = 0; i < 48; i++)
	{
		if (!forward_table[i].PRIMARY || i == prev - 1)
			continue;

		op_pk_nfd_set_int32(q, "PREV", id);
		LOGD("[%d]forward_table[%d].PRIMARY = %d", id, i, forward_table[i].PRIMARY);
		r = op_pk_copy(q);
		if (forward_table[i].MPR == false)
		{
			op_pk_nfd_set_int32(q, "CTRL", 0);
		}
		s = op_pk_copy(p);
		op_pk_nfd_set_pkt(s, "data", r);
		opnet_pri_packet ss(s, prio);
		txQ[i].push(ss);
		LOG("[%d]BROADCAST PUSH txQ[%d]", id, i);
	}
	op_pk_nfd_set_pkt(p, "data", q);
	// op_pk_print(p);
}

void dtdma::proc_unicast(Packet *q)
{
	int src, dst, type, prio;
	Packet *p = OPC_NIL, *r = OPC_NIL, *s = OPC_NIL;
	if (q == OPC_NIL)
		return;
	p = op_pk_copy(q);
	op_pk_nfd_get_int32(p, "dst", &dst);
	op_pk_nfd_get_int32(p, "src", &src);
	op_pk_nfd_get_int32(p, "prio", &prio);
	op_pk_nfd_get_int32(p, "type", &type);

	int fwd = forward_table[dst - 1].id;

	if (type == ETHERTYPE_IP)
	{
		int pkid = 0;
		op_pk_nfd_get_pkt(p, "data", &r);
		op_pk_fd_get_int32(r, 0, &pkid);
		LOG("debug:pkid:%d", pkid);
	}
	

	if (fwd != 0 && type == ETHERTYPE_IP)
	{
		int pkid = 0;
		op_pk_nfd_get_pkt(p, "data", &r);
		op_pk_fd_get_int32(r, 0, &pkid);
		op_pk_nfd_set_pkt(p, "data", r);
		LOG("[%d][%d]RELAY:[%d]->[%d] via [%d]", id, pkid, src, dst, fwd);
	}

	if (fwd != 0)
	{
		txQ[fwd - 1].push(opnet_pri_packet(p, prio));
	}
	else
	{
		op_pk_destroy(p);
	}
}

// This function encapsulate packet p with broadcast header
Packet *dtdma::encap_broadcast(Packet *p)
{
	my_seq_num++; // increase sequence number
	Packet *q = op_pk_create_fmt("724BROADCAST");
	op_pk_nfd_set_int32(q, "HTL", 12);
	op_pk_nfd_set_int32(q, "PREV", id);
	op_pk_nfd_set_int64(q, "SEQ", my_seq_num);
	op_pk_nfd_set_pkt(q, "DATA", p);
	return q;
}

Packet *dtdma::encap_mac(Packet *p, int dst, int prio, int type)
{
	Packet *r = op_pk_create_fmt("724mac");
	op_pk_nfd_set_int32(r, "dst", dst);
	op_pk_nfd_set_int32(r, "src", id);
	op_pk_nfd_set_int32(r, "prio", prio);
	op_pk_nfd_set_int32(r, "type", type);
	op_pk_nfd_set_pkt(r, "data", p);
	return r;
}

void dtdma::send_queue_info()
{
	Packet *p = op_pk_create(0);
	for (int i = 0; i < 48; i++)
	{
		op_pk_fd_set_int64(p, i, txQ[i].calc_size(), 64);
	}
	op_pk_send(p, RRC_BALLOC_REQ);
}

opnet_pri_packet::opnet_pri_packet(Packet *p, int prio)
{
	this->opnet_packet = p;
	this->data = (void *)p;
	this->size = op_pk_total_size_get(p);
	this->priority = prio;
}

opnet_pri_packet::~opnet_pri_packet() {}

void *opnet_pri_packet::get_packet() const
{
	return (void *)opnet_packet;
}

unsigned long opnet_pri_queue::calc_size() const
{
	return transfer_queue::calc_size();
}

unsigned long opnet_pri_queue::calc_size(dtdma_ns::prio_t prio) const
{
	return buf_size_by_prio[static_cast<int>(prio)];
}

opnet_pri_queue::opnet_pri_queue()
{
	transfer_queue();
	for (int i = 0; i <= UCUCH; i++)
	{
		buf_size_by_prio.push_back(0);
	}
};

void opnet_pri_queue::push(const pri_packet_t &p)
{
	buf_size_by_prio[p.get_priority()] += p.get_size();
	transfer_queue::push(p);
}

void opnet_pri_queue::pop()
{
	buf_size_by_prio[top().get_priority()] -= top().get_size();
	transfer_queue::pop();
}