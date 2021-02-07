#pragma once

#include <queue>
#include <set>
#include <map>
#include <algorithm>

struct link
{
    public:
    int n_first;
    int n_second;
    int color;
    link(std::pair<int, int> n) : n_first(n.first), n_second(n.second), color(0){};
    link(){};
    // bool operator<(link b)const{return this->n_first < b.n_first;};
};

typedef class vertex
{
public:
    virtual int get_id() const = 0;
    vertex(int id);
    // virtual bool operator==(const vertex &v) const = 0;
    // virtual bool operator<(const vertex &v) const = 0;
    // virtual bool operator>(const vertex &v) const = 0;
protected:
    int id;
} vertex_t;

template <class vertex>
class edge
{
protected:
    std::pair<vertex, vertex> vertice;

public:
    edge(const vertex &a, const vertex &b);
    virtual bool operator==(const edge &e) const;
    virtual bool operator>(const edge &e) const;
    virtual bool operator<(const edge &e) const;
    virtual bool operator!=(const edge &e) const;
    virtual double get_weight() const;
    virtual std::pair<vertex, vertex> get_vertice() const;
};

template <class vertex, class edge>
class router
{
public:
    router<vertex, edge>();
    ~router();
    void add_edge(const vertex &a, const vertex &b);
    void add_vertex(const vertex &a);
    void remove_vertex(const vertex &a);

    virtual void Dijkstra(const vertex &v);                     // Update route and calc MPR and forwarding table
    virtual std::set<vertex> get_mpr_nodes() const;             // get MPR nodes
    virtual std::map<vertex, vertex> get_forward_table() const; //get forwarding table
    virtual std::pair<vertex, bool> get_vertex(int id) const;
    virtual std::set<vertex> get_vertice() const;

private:
    std::set<vertex> vertice;
    std::set<std::pair<vertex, vertex>> edge_vertice;
    std::map<std::pair<vertex, vertex>, edge> edges;
    std::set<vertex> mpr_nodes;
    std::map<vertex, vertex> forward_table;
};
