#include <state.hpp>

namespace rlc_ns{


enum{
    ON_PDCP_DL,
    ON_MAC_UL
}intrpt_t;

enum{
    PDCP_DL,
    MAC_UL,
    PDCP_UL,
    MAC_DL
}strm_t;
typedef class manager{
    public:
        manager();
        ~manager();
    private:
        int id;

}manager_t;

typedef class idle:state{
    public:
        idle(manager_t *mngr);
        ~idle();
        void proc(int cond);
        bool cond(int cond);
        void enter();
        void exit();
    private:
        manager_t * parent;
}idle_t;

};