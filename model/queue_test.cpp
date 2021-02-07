#include <transfer_queue.hpp>
#include <iostream>

using namespace std;
class mypacket:public pri_packet
{
    public:
        mypacket(int p){
            size = 0;
            priority = p;
        };
        ~mypacket(){

        };

};

int main(int argc,char *argv[])
{
    transfer_queue q;
    mypacket a(1),b(2),c(3),d(4),e(5);
    
    q.push(a);
    q.push(b);
    q.push(c);
    q.push(d);
    q.push(e);

    while(q.size()!=0){
        printf("%d ",q.top().get_priority());
        q.pop();
    }


}