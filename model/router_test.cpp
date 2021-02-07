#include <router.hpp>
#include <node.hpp>
#include <float.h>
#include <iostream>

int main(int argc, char *argv[])
{
    node_t a(1), b(2), c(3), d(4), e(5), f(6);

    a.rti[0] = 1;
    b.rti[0] = 0;
    a.bov[0] = b.get_id();
    b.bov[0] = a.get_id();

    b.rti[1] = 1;
    c.rti[1] = 0;
    b.bov[1] = c.get_id();
    c.bov[1] = b.get_id();

    c.rti[2] = 1;
    d.rti[2] = 0;
    c.bov[2] = d.get_id();
    d.bov[2] = c.get_id();

    d.rti[3] = 1;
    f.rti[3] = 0;
    d.bov[3] = f.get_id();
    f.bov[3] = d.get_id();

    c.rti[4] = 1;
    e.rti[4] = 0;
    c.bov[4] = e.get_id();
    e.bov[4] = c.get_id();

    e.rti[5] = 1;
    f.rti[5] = 0;
    e.bov[5] = f.get_id();
    f.bov[5] = e.get_id();

    b.rti[6] = 1;
    f.rti[6] = 0;
    b.bov[6] = f.get_id();
    f.bov[6] = b.get_id();

    a.rti[7] = 1;
    d.rti[7] = 0;
    a.bov[7] = d.get_id();
    d.bov[7] = a.get_id();

    router<node, edge<node>> r;

    r.add_vertex(a);
    r.add_vertex(b);
    r.add_vertex(c);
    r.add_vertex(d);
    r.add_vertex(e);
    r.add_vertex(f);

    r.Dijkstra(a);

    std::map<node, node> table = r.get_forward_table();
    std::set<node> mpr = r.get_mpr_nodes();
}