
#include <transfer_queue.hpp>

using namespace std;
transfer_queue::transfer_queue() : buffer_size(0) {}

transfer_queue::~transfer_queue() {}

void transfer_queue::push(const pri_packet_t &p)
{
    priority_queue<pri_packet>::push(p);
    buffer_size += p.get_size();
}

void transfer_queue::pop()
{
    buffer_size -= top().get_size();
    priority_queue<pri_packet>::pop();
}

unsigned long transfer_queue::calc_size() const
{
    return buffer_size;
}
