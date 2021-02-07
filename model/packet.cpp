#include <packet.hpp>

bool pri_packet::operator==(const pri_packet &p) const
{
    return this->priority == p.get_priority();
}
bool pri_packet::operator>(const pri_packet &p) const
{
    return this->priority > p.get_priority();
}
bool pri_packet::operator<(const pri_packet &p) const
{
    return this->priority < p.get_priority();
}
unsigned long pri_packet::get_size() const
{
    return this->size;
}
unsigned char pri_packet::get_priority() const
{
    return this->priority;
}

void *pri_packet::get_packet() const
{
    return data;
}