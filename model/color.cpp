#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<set>

using namespace std;

struct link
{
    public:
    int n_first;
    int n_second;
    int color;
    link(pair<int, int> n) : n_first(n.first), n_second(n.second), color(0){};
    link(){};
    // bool operator<(link b)const{return this->n_first < b.n_first;};
};

pair<set<int>, int> cal_S(link n, vector<link> V){
    set<int> s;
    for (auto i = V.begin(); i != V.end(); i++)
    {
        if ((i->n_first == n.n_first || i->n_first == n.n_second || i->n_second == n.n_first || i->n_second == n.n_second) && i->color != 0)
        {
            s.insert(i->color);
        }
    }
    return make_pair(s, s.size());
}

int find_min_num(set<int> s)
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

bool colored(vector<link> s)
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

int main()
{
    pair<int,int> p1(1,2), p2(1,3), p3(1,4), p4(1,5);
    vector<link> V;
    V.push_back(link(p1));
    V.push_back(link(p2));
    V.push_back(link(p3));
    V.push_back(link(p4));
    while (colored(V))
    {
        int S_max = -1;
        // link candidate;
        vector<link> :: iterator tmp;
        set<int> stmp;
        pair<set<int>, int> res;
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
        cout << "(" << tmp->n_first << "," << tmp->n_second << ")" << tmp->color << endl;
    }
}




