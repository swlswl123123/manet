#include <node.hpp>
#include <string.h>

using namespace balloc;

node::node() : vertex(0)
{
    last_update = 0;
    memset(&bov, 0x00, sizeof(bov));
}

node::node(const beacon_info_t &info, unsigned long long time) : vertex(info.id)
{
    this->last_update = time;
    this->bov = info;
}

node::~node(){};
node::node(int id) : vertex(id)
{
    last_update = 0;
    memset(&bov, 0x00, sizeof(bov));
    this->bov.id = id;
};
int node::get_id() const
{
    return this->id;
};

void node::update(beacon_info_t &info, unsigned long long time)
{
    if (time > this->last_update)
    {
        this->last_update = time;
        this->bov = info;
    }
}

bool node::operator==(const node_t &n) const
{
    return this->id == n.get_id();
};
bool node::operator>(const node_t &n) const
{
    return this->id > n.get_id();
};
bool node::operator<(const node_t &n) const
{
    return this->id < n.get_id();
};

//Calculate common beacon count
int node::operator|(const node_t &b) const
{
    return (*this || b) + (b || *this);
}

int node::operator||(const node_t &b) const
{
    int ret = 0;
    for (int i = 0; i < 62; i++)
    {
        // LOG("[%d]BEACON_TABLE[%d] id = %d RTI = %d MODE = %d", this->get_id(), i, this->get_bov().links[i].id, this->get_bov().links[i].mode, this->get_bov().links[i].type);
        // LOG("[%d]BEACON_TABLE[%d] id = %d RTI = %d MODE = %d", b.get_id(), i, b.get_bov().links[i].id, b.get_bov().links[i].mode, b.get_bov().links[i].type);
        if (this->bov.links[i].id == b.id && b.bov.links[i].id == this->id && this->bov.links[i].mode == SEND && b.bov.links[i].mode == RECV)
        {
            ret++;
            // LOG("change incre");
        }
    }
    return ret;
}

balloc::beacon_info_t node::get_bov() const
{
    return bov;
}