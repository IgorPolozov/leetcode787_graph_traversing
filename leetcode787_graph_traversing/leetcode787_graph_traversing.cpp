#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <utility>//std::pair

using ui_t=int;
struct node
{
    ui_t id;
    int mutable visited;

    bool operator==(const node &other)const
    {
        return id==other.id;
    }   

    void set_visited()const
    {
         visited=1;
    }

     bool is_visited_by()const
    {
        return visited;
    }
   
};

struct Hash_node
{
    int operator()(const node& n)const
    {
        return n.id;
    }
};

struct pnode
{
    const node *  nd;

    bool operator==(const pnode &other)const
    {
        return *nd==*other.nd;
    }
};

 struct Hash_pnode
{
    int operator()(const pnode& pn)const
    {
        return pn.nd->id;
    }
};

struct flight
{
     pnode pn;
     ui_t price;

};

class Solution//leetcode 787 task: https://leetcode.com/problems/cheapest-flights-within-k-stops/description/
{
using vec_flight= std::vector<flight>;
using vec_flight_iter=vec_flight::iterator;
using vec_flight_citer=vec_flight::const_iterator;

using mapa =std::unordered_map<pnode, std::vector<flight>, Hash_pnode>;
using mapa_iter =mapa::iterator;
using mapa_citer=mapa::const_iterator;

using set=std::unordered_set<node, Hash_node>;

struct stack_args
               {
                  vec_flight_iter
                  it_vec_begin//,it_vec_end
                  ;
                  mapa_iter it;
                  int sits_count;
                  int tmp;
                };
public:
int cycles_counter=0;
//parameter n is not necessary, because it is determined by the flights
int findCheapestPrice(const int& n, const std::vector<std::vector<ui_t>>& flights, const int& src, const int& dst, const int& k)
{
    //prepare data structures:
    set nodes;

    for(auto elem:flights)
    {
        nodes.insert({elem[0],0});// id, not visited to node ctor
        nodes.insert({elem[1],0});//
    }

    mapa pnodes;

    for(auto &elem:flights)
    {
       node from {elem[0],0};
       auto it=nodes.find(from);
       const pnode pfrom{&*it};

       node to {elem[1],0};
            it=nodes.find(to);
       const pnode pto{&*it};

       auto key_it=pnodes.find(pfrom);

       if(key_it==pnodes.end()){
       pnodes.insert( { pfrom, {} }  );
       }

       key_it=pnodes.find(pfrom);
       flight fl{pto, elem[2]};
       key_it->second.push_back( fl );
    }

    for(auto &elem:pnodes)
    {//decreases the sycles number inmproving performance on a big cities links number
      std::sort(elem.second.begin(), elem.second.end(),
                [](const flight &a, const flight b){return a.price<b.price;});

    }

    node    sorc= {ui_t(src), 0},
            dest= {ui_t(dst), 0};

    auto it_target=nodes.find(sorc);

    if(it_target==nodes.end())
    {
        //std::cout<<"Not matched source node in nodes!\n";
        return -1;
    }

    auto it_dest=nodes.find(dest);
        if(it_dest==nodes.end())
        {
            //std::cout<<"Not matched destination node in nodes!\n";
            //std::cout<<dest<<'\n';
            return -1;
        }

    const pnode pstart{&*it_target};
    pstart.nd->visited=1;
    const
    pnode pdest{&*it_dest};


//////////init block
mapa_iter//initialy all are equal
            it=pnodes.find(pstart);
mapa_citer  cit_start = it;

            if(it==pnodes.end()){
            //std::cout<<"Not matched source node in pnodes!\n";
            return -1;
            }

vec_flight_iter
            it_vec_begin=   it->second.begin(),
            it_vec_end=     it->second.end();

             int sits_max=k;

               int  sits_count=0,
                    tmp=0,
                    result=INT_MAX;

               stack_args starting_args{it_vec_begin, it, sits_count, tmp};

               std::stack<stack_args> args;
               args.push(starting_args);
               //end of intit block

               int is_visited=0;//helping variable related to while(true)

               while(true)
               {
                 //  std::cout<<"root node= "<<(args.top().it->first.nd->id)<<"\n";
                 //  std::cin.get();
                   cycles_counter++;

                   while(!args.empty() && (args.top().it_vec_begin==args.top().it->second.end()) )//pop off all inaccessible ones
                   {
                        args.top().it->first.nd->visited=0;
                        args.pop();
                   }

                   if(args.empty()) break;//final point

                   auto &args_obj=args.top();//this var slightly decreases performance, but it is too useful to have clear code

                   tmp=args_obj.tmp;
                   sits_count=args_obj.sits_count;


                   it_vec_begin=args_obj.it_vec_begin;
                   if(
                      (  (tmp+it_vec_begin->price)  >  result  )
                      ||
                      (sits_count>sits_max)
                      )
                   {
                     ++args.top().it_vec_begin;//go right
                            continue;
                   }

                   it_vec_end  =args_obj.it->second.end();

                   it=args_obj.it;

                   ///////reinit visited in nodes when it == cit_start i.e. at the rout start

                   if(it==cit_start){
                        for(auto &elem:nodes)
                        {
                            elem.visited=0;
                        }
                           pstart.nd->visited=1;
                            tmp=0;
                            sits_count=0;
                   }

                       is_visited=it_vec_begin->pn.nd->is_visited_by();

                   if(is_visited){
                   ++args.top().it_vec_begin;//arg must incremented to next second to start new step of cycle
                   continue;
                   }

                    if(*it_vec_begin->pn.nd==*const_cast<const node*>(pdest.nd))//if destination is achieved
                    {
                            tmp+=it_vec_begin->price;
                            result=tmp<result?tmp:result;
                            ++args.top().it_vec_begin;//go right to find longer but chipper variant
                            continue;
                    }

//this if is excluded from pgm and all works fine, because here can not be  it_vec_begin==it_vec_end
                    if( it_vec_begin!=it_vec_end ){ //otherwice next step of while(true) cycle


                    it=pnodes.find(it_vec_begin->pn);//try to

                    if(it==pnodes.end()){//if failed to go straight ahead -> try to go RIGHT
                    //leaf node - reason is that node is not present in pnodes as the starting one
                      ++args.top().it_vec_begin;
                      continue;
                     }//end of if(it==pnodes.end()) to move right
                     else{//can go FORWARD
                    //here - set_visited, tmp+=price, ++sits_count to create new arg for pushing

                    it_vec_begin->pn.nd->set_visited();//here only
                    //increment old it_vec_begin to go right after poping previous arg
                    ++args.top().it_vec_begin;

                   vec_flight_iter new_it_vec_begin=it->second.begin();
                   ++sits_count;//here only
                   tmp+=it_vec_begin->price;//here only to save in args.

                   stack_args new_arg{new_it_vec_begin, it, sits_count, tmp};
                   args.push(new_arg);
                   continue;
                     }//end else of can go FORWARD by the reason of it!=pnodes.end()
                  }//end of if( it_vec_begin!=it_vec_end  && (sits_count<=sits_max) )
               }//end while(true)

return result==INT_MAX?-1:result;
}
};
