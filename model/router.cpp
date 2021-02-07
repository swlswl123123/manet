#include <router.hpp>
#include <node.hpp>
#include <set>
#include <float.h>
#include <iostream>
#include <limits.h>
#include <deque>

bool colored(std::vector<link> s)
{
    for (auto i = s.begin(); i != s.end(); i++)
    {
        if (i->color == 0)
        {
            return true;
        }
    }
    return false;
}

int find_min_num(std::set<int> s)
{
    if (s.empty())
    {
        return 1;
    }
    
    for (int i = 1; i < 10; i++)
    {
        auto res = s.find(i);
        if (res == s.end())
        {
            return i;
        }
    }
}

std::pair<std::set<int>, int> cal_S(link n, std::vector<link> V){
    std::set<int> s;
    for (auto i = V.begin(); i != V.end(); i++)
    {
        if ((i->n_first == n.n_first || i->n_first == n.n_second || i->n_second == n.n_first || i->n_second == n.n_second) && i->color != 0)
        {
            s.insert(i->color);
        }
    }
    return std::make_pair(s, s.size());
}

vertex::vertex(int id)
{
    this->id = id;
}

template <class vertex>
edge<vertex>::edge(const vertex &a, const vertex &b)
{
    this->vertice.first = a;
    this->vertice.second = b;
};

template <class vertex>
bool edge<vertex>::operator==(const edge &b) const
{
    return this->get_weight() == b.get_weight();
};

template <class vertex>
bool edge<vertex>::operator>(const edge &b) const
{
    return this->get_weight() >= b.get_weight();
};

template <class vertex>
bool edge<vertex>::operator<(const edge &b) const
{
    return this->get_weight() < b.get_weight();
};

template <class vertex>
bool edge<vertex>::operator!=(const edge &b) const
{
    return this->get_weight() != b.get_weight();
};

template <class vertex>
double edge<vertex>::get_weight() const
{
    int width = this->vertice.first || this->vertice.second;
    // printf("[%d]->[%d] W = %d\n",this->vertice.first.get_id(),this->vertice.second.get_id(),width);

    return width == 0 ? DBL_MAX : 1.0 / (double)(width);
}

template <class vertex>
std::pair<vertex, vertex> edge<vertex>::get_vertice() const
{
    return this->vertice;
}

template <class vertex, class edge>
router<vertex, edge>::router(){

};

template <class vertex, class edge>
router<vertex, edge>::~router(){

};

template <class vertex, class edge>
void router<vertex, edge>::add_edge(const vertex &a, const vertex &b)
{
    auto n = vertice.find(a);
    if (n != vertice.end())
    {
        vertice.erase(a);
    }
    vertice.insert(a);

    n = vertice.find(b);
    if (n == vertice.end())
    {
        vertice.erase(b);
    }
    vertice.insert(b);

    edge e(a, b);

    if (edge_vertice.find(std::make_pair(a, b)) == edge_vertice.end())
    {
        printf("edges not found!\n");
        edge_vertice.insert(std::make_pair(a, b));
        edges.insert(std::make_pair(std::make_pair(a, b), e));
    }

    for (auto e = edges.begin(); e != edges.end(); e++)
    {
        printf("[%d]->[%d]:W =%lf\n", e->first.first.get_id(), e->first.second.get_id(), e->second.get_weight());
    }
}

template <class vertex, class edge>
void router<vertex, edge>::add_vertex(const vertex &a)
{
    auto n = vertice.insert(a);
    if (!n.second)
    {
        auto m = n.first++;
        vertice.erase(m);
        vertice.insert(n.first, a);
    }
}

template <class vertex, class edge>
void router<vertex, edge>::remove_vertex(const vertex &a)
{
    vertice.erase(a);
}

template <class vertex, class edge>
void router<vertex, edge>::Dijkstra(const vertex &from)
{

    int N = vertice.size();
    LOG("[%d]num of vertice: %d", from.get_id(), N);
    vertex **vs = new vertex *[N];
    bool *visited = new bool[N];
    int *prev = new int[N];
    double *dist = new double[N];
    int *hops = new int[N];
    double **weights = new double *[N];
    int *fwd = new int[N];
    bool *MPR = new bool[N];
    int *coverage = new int[N];

    for (int i = 0; i < N; i++)
    {
        weights[i] = new double[N];
        coverage[i] = 0;
    }

    int i = 0;
    int src = -1;
    int self_id = 0;
    for (auto v = vertice.begin(); v != vertice.end(); v++, i++)
    {
        vs[i] = (vertex *)&(*v);
        prev[i] = -1;
        dist[i] = DBL_MAX;
        visited[i] = false;
        hops[i] = 65535;
        MPR[i] = false;

        if (*v == from)
        {
            visited[i] = true;
            dist[i] = 0;
            hops[i] = 0;
            prev[i] = i;
            src = i;
            self_id = i;
        }
    }

    // printf("W = [\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (j == i)
            {
                weights[i][j] = 0;
            }
            else
            {
                weights[i][j] = edge(*vs[i], *vs[j]).get_weight();
            }
            // printf("weights[%d][%d] = %3.2lf\n",vs[i]->get_id(),vs[j]->get_id(),weights[i][j]);
        }
        // printf("\n");
    }
    // printf("]\n");
    
    /*color problem*/
    std::vector<int> s;
    std::vector<link> V;
    int myid = vs[self_id]->get_id();

    for (int i = 0; i < N; i++)
    {
        if (weights[self_id][i] != DBL_MAX && i != self_id)
        {
            s.push_back(i);
            V.push_back(link(std::make_pair(myid, vs[i]->get_id())));
        }
    }

    for (int i = 0; i < s.size(); i++)
    {
        for (int j = i + 1; j < s.size(); j++)
        {
            if (weights[s[i]][s[j]] != DBL_MAX)
            {
                V.push_back(link(std::make_pair(vs[s[i]]->get_id(), vs[s[j]]->get_id())));
            }
        }
    }
    
    while (colored(V))
    {
        int S_max = -1;
        // link candidate;
        std::vector<link> :: iterator tmp;
        std::set<int> stmp;
        std::pair<std::set<int>, int> res;
        for (auto i = V.begin();i != V.end();i++)
        {
            if (i->color == 0)
            {
                res = cal_S(*i, V);
                if (res.second > S_max)
                {
                    S_max = res.second;
                    stmp = res.first;
                    // cout << "S_max:" << S_max << endl;
                    tmp = i;
                }
            }
        }
        tmp->color = find_min_num(stmp);

        // for (auto i = V.begin(); i != V.end() ; i++)
        // {
        //     cout << "V_col:" << i->color << endl;
        // }
        // cout << "(" << tmp->n_first << "," << tmp->n_second << ")" << tmp->color << endl;
        LOG("[%d](%d,%d)color:%d", from.get_id(), tmp->n_first, tmp->n_second, tmp->color);
    }
    

    int cnt_visited = 0;
    int cnt_visited_tmp = cnt_visited;
    do
    {
        cnt_visited_tmp = cnt_visited;
        for (int n = 0; n < N; n++)
        {
            if (!visited[n])
                continue;

            for (int m = 0; m < N; m++)
            {
                if (weights[n][m] != DBL_MAX && !visited[m])
                {
                    if (dist[n] + weights[n][m] < dist[m])
                    {
                        prev[m] = n;
                        dist[m] = dist[n] + weights[n][m];
                    }

                    if (hops[n] + 1 < hops[m])
                    {

                        hops[m] = hops[n] + 1;
                    }

                    visited[m] = true;
                    cnt_visited++;
                }
            }
        }

    } while (cnt_visited != cnt_visited_tmp);

    //Update forwarding table
    forward_table.clear();

    //Initialize forward table
    for (int i = 0; i < N; i++)
    {
        fwd[i] = -1;
    }

    //Loop over nodes
    for (int i = 0; i < N; i++)
    {
        //Exclude unreachable nodes, source node & already computed nodes
        if (i == src || hops[i] == 65535 || fwd[i] != -1)
            continue;

        std::deque<int> q;
        int j = i;

        do
        {
            q.push_back(j);
            j = prev[j];
        } while (j != src);

        while (q.size() > 0)
        {
            fwd[q.front()] = q.back();
            q.pop_front();
        }
    }

    //Put the result into map
    for (int i = 0; i < N; i++)
    {
        if (i == src || fwd[i] == -1)
            continue;
        forward_table.insert(std::make_pair(*vs[i], *vs[fwd[i]]));
        printf("[%d]->[%d] via:%d R = %lf H = %d\n", from.get_id(), vs[i]->get_id(), vs[fwd[i]]->get_id(), dist[i], hops[i]);

        //reset for MPR calc
        visited[i] = false;
    }

    mpr_nodes.clear();

    for (int i = 0; i < N; i++)
        visited[i] = false;

    for (int i = 0; i < N; i++)
    {
        if (hops[i] != 2)
            continue;

        int last_index = 0;
        int cover = 0;
        for (int j = 0; j < N; j++)
        {
            if (hops[j] != 1)
                continue;
            if (weights[j][i] < DBL_MAX)
            {
                cover++;
                last_index = j;
            }
        }

        if (cover == 1)
        {
            MPR[last_index] = true;
            for (int j = 0; j < N; j++)
            {
                if (hops[j] != 2)
                    continue;
                if (weights[last_index][j] < DBL_MAX)
                {
                    visited[j] = true;
                }
            }
        }
    }

    do
    {

        std::priority_queue<std::pair<int, int>> q;
        for (int i = 0; i < N; i++)
        {
            coverage[i] = 0;
        }
        //Calculate MPR
        for (int i = 0; i < N; i++)
        {
            if (hops[i] != 1 || MPR[i])
                continue;

            for (int j = 0; j < N; j++)
            {
                if (hops[j] != 2)
                    continue;

                if (weights[i][j] < DBL_MAX && visited[j] == false)
                {
                    coverage[i]++;
                }
            }

            if (coverage[i] != 0)
            {
                q.push(std::pair<int, int>(i, coverage[i]));
            }
        }

        if (q.size() > 0)
        {
            MPR[q.top().first] = true;
            for (int i = 0; i < N; i++)
            {
                if (hops[i] != 2)
                    continue;
                if (weights[q.top().first][i] < DBL_MAX)
                {
                    visited[i] = true;
                }
            }
        }
        else
        {
            break;
        }

    } while (1);

    for (int i = 0; i < N; i++)
    {
        if (MPR[i])
        {
            mpr_nodes.insert(*vs[i]);
        }
    }

    //Resource dealloc
    delete[] vs;
    delete[] prev;
    delete[] dist;
    delete[] visited;
    delete[] hops;
    delete[] weights;
    delete[] fwd;
    delete[] MPR;
}

template <class vertex, class edge>
std::set<vertex> router<vertex, edge>::get_mpr_nodes() const
{
    return mpr_nodes;
}

template <class vertex, class edge>
std::map<vertex, vertex> router<vertex, edge>::get_forward_table() const
{
    return forward_table;
}

template <class vertex, class edge>
std::pair<vertex, bool> router<vertex, edge>::get_vertex(int id) const
{
    auto v = vertice.find(vertex(id));
    std::pair<vertex, bool> ret;
    if (v == vertice.end())
    {
        // printf("router:get_vertex(%d) = NOTFOUND!\n",id);
        for (auto u = vertice.begin(); u != vertice.end(); u++)
        {
            // printf("vertices = %d\n", u->get_id());
        }
        ret.second = false;
    }
    else
    {
        ret.second = true;
        // printf("router:get_vertex(%d) = FOUND!\n",id);
        ret.first = *v;
    }
    return ret;
}
template <class vertex, class edge>
std::set<vertex> router<vertex, edge>::get_vertice() const
{
    return vertice;
}

template class edge<node>;
template class router<node, edge<node>>;