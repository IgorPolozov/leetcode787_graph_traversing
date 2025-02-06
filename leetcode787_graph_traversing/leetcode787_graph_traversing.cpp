#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>

class Solution//leetcode 787 task: https://leetcode.com/problems/cheapest-flights-within-k-stops/description/
{
    using ui_t = int;//I used unsigned at start and in the finish understood that it will not fit litcode signature

    struct node
    {
        ui_t id;
        node mutable* visited;
        bool operator==(const node& other)const
        {
            return id == other.id;
        }
        void set_visited_by(const node& other)const
        {
            visited = const_cast<node*>(&other);
        }

        void prn_visited_by(const node& other)const//for debug)
        {
            std::cout << "\nlist of visitors of node : " << id << "\n";
            const node* ptr_node = visited;
            while (ptr_node)
            {
                std::cout << ptr_node->id << '-';
                ptr_node = ptr_node->visited;
            }
            std::cout << '\n';
        }

        bool is_visited_by(const node& other)const
        {
            bool result = false;
            const node* ptr_node = &other;
            while (ptr_node)
            {
                if (*ptr_node == *this) {
                    result = true;
                    break;
                }
                ptr_node = ptr_node->visited;
            }
            return result;
        } 
    }; 

    struct Hash_node
    {
        int operator()(const node& n)const
        {
            return int(n.id);
        }
    };

    struct pnode
    {
        const node* nd;
        bool operator==(const pnode& other)const
        {
            return *nd == *other.nd;
        } 
    };
    
    struct Hash_pnode
    {
        int operator()(const pnode& pn)const
        {
            return Hash_node()(*pn.nd);
        }
    };

    struct flight
    {
        pnode pn;
        unsigned price;

    };
    
    using vec_flight = std::vector<flight>;
    using vec_flight_iter = vec_flight::iterator;
    using vec_flight_citer = vec_flight::const_iterator;

    using mapa = std::unordered_map<pnode, std::vector<flight>, Hash_pnode>;
    using mapa_iter = mapa::iterator;
    using mapa_citer = mapa::const_iterator;

    using set = std::unordered_set<node, Hash_node>;

    struct stack_args
    {
        vec_flight_iter
            it_vec_begin,
            it_vec_end;
        mapa_iter it;
        int sits_count;
        int tmp;
        node* prev_visited;
    };

public:
    //parameter n is not necessary, because it is determined by the flights
    int findCheapestPrice(const int& n, const std::vector<std::vector<ui_t>>& flights, const int& src, const int& dst, const int& k)
    {
        //prepare data structures:
        set nodes;

        for (auto elem : flights)
        {
            nodes.insert({ elem[0],0 });// id, not visited to node ctor
            nodes.insert({ elem[1],0 });//
        }

        mapa pnodes;        

        for (auto elem : flights)
        {
            node from{ elem[0],0 };
            auto it = nodes.find(from);
            const pnode pfrom{ &*it };

            node to{ elem[1],0 };
            it = nodes.find(to);
            const pnode pto{ &*it };

            auto key_it = pnodes.find(pfrom);

            if (key_it == pnodes.end()) {
                pnodes.insert({ pfrom, {} });
            }
            
            key_it = pnodes.find(pfrom);
            flight fl{ pto, elem[2] };
            key_it->second.push_back(fl);
        }
        for(auto &elem:pnodes)
        {
          std::sort(elem.second.begin(), elem.second.end(), [](const flight &a, const flight b){return a.price<b.price;});    
        }

        node    sorc = { ui_t(src), 0 },
            dest = { ui_t(dst), 0 };

        auto it_target = nodes.find(sorc);

        if (it_target == nodes.end()){
            //std::cout<<"Not matched source node in nodes!\n";
            return -1;
        }

        auto it_dest = nodes.find(dest);
        if (it_dest == nodes.end()){
            //std::cout<<"Not matched destination node in nodes!\n";
            //std::cout<<dest<<'\n';
            return -1;
        }
        
        const pnode pstart{ &*it_target };
        const pnode pdest{ &*it_dest };
        //////////init block
        mapa_iter//initialy all are equal
            it = pnodes.find(pstart);
        mapa_citer  cit_start = it;

        if (it == pnodes.end()) {
            //std::cout<<"Not matched source node in pnodes!\n";
            return -1;
        }
        
        vec_flight_iter
            it_vec_begin = it->second.begin(),
            it_vec_end = it->second.end();

        vec_flight_citer it_vec_fin = it_vec_end;//to make reinit visited and tmp at any rout begin

        int sits_max = k//-1
            ;//last lending not counted

        int  sits_count = 0,//first starting point not counted and incremented in the finish of forward step
            tmp = 0,
            result = INT_MAX;
        node* prev_visited = nullptr;

        stack_args starting_args{ it_vec_begin, it_vec_end, it, sits_count, tmp, prev_visited };

        std::stack<stack_args> args;
        args.push(starting_args);
        //end of intit block

        bool is_visited = false;//helping variable related to while(true) Here and further all intermediete vars are used for clarity only
        while (true)
        {
            while (!args.empty() && (args.top().it_vec_begin == args.top().it_vec_end))//pop off all inaccessible ones
            {
                args.top().it->first.nd->visited = args.top().prev_visited;
                args.pop();
            }
            //clearing args from arg(s) whos vec<flights> is finished

            if (args.empty()) break;//final point 

            auto& args_obj = args.top();//this var slightly decreases performance, but it is too useful to have clear code

            tmp = args_obj.tmp;
            sits_count = args_obj.sits_count;

            it_vec_begin = args_obj.it_vec_begin;
            if (
                ((tmp + it_vec_begin->price) > result)
                ||
                (sits_count > sits_max)
                )
                {
                    ++args.top().it_vec_begin;//go right
                    continue;
                }

            it_vec_end = args_obj.it_vec_end;

            it = args_obj.it;            

            ///////reinit visited in nodes when it == cit_start i.e. at the rout start

            if (it == cit_start) {
                for (auto& elem : nodes)
                {
                    elem.visited = 0;
                }
                tmp = 0;
                sits_count = 0;
            }
            
            //check visiting to prevent cycling
            const node* current_ptr = const_cast<const node*>(it->first.nd);
            const node& current_nd = *current_ptr;
            //check if the second was visited through the first one
            is_visited = it_vec_begin->pn.nd->is_visited_by(current_nd);

            if (is_visited) {
                ++args.top().it_vec_begin;//arg must incremented to next second to start new step of cycle
                continue;
            }
            else {//check if first was visited by second
                is_visited = current_nd.is_visited_by(*it_vec_begin->pn.nd);
                if (is_visited) {
                    ++args.top().it_vec_begin;//there and further
                    continue;
                }
            }

            if (*it_vec_begin->pn.nd == *const_cast<const node*>(pdest.nd))//if destination is achieved
            {
                tmp += it_vec_begin->price;
                result = tmp < result ? tmp : result;
                ++args.top().it_vec_begin;//go right to find longer but chipper variant
                continue;
            }

            if (it_vec_begin != it_vec_end) { //otherwice next step of while(true) cycle

                it = pnodes.find(it_vec_begin->pn);//try to

                if (it == pnodes.end()) {//if failed to go straight ahead -> try to go RIGHT
                //leaf node - reason is that node is not present in pnodes as the starting one
                   ++args.top().it_vec_begin;                   
                   
                    continue;
                }//end of if(it==pnodes.end()) to move right
                else {//can go FORWARD
               //here - set_visited, tmp+=price, ++sits_count to create new arg for pushing
                    node* prev_visited_to_save = it_vec_begin->pn.nd->visited;
                    it_vec_begin->pn.nd->set_visited_by(current_nd);//here only
                    //increment old it_vec_begin to go right after poping previous arg
                    ++args.top().it_vec_begin;

                    vec_flight_iter new_it_vec_begin = it->second.begin(),
                        new_it_vec_end = it->second.end();
                    ++sits_count;//here only
                    tmp += it_vec_begin->price;//here only to save in args.

                    stack_args new_arg{ new_it_vec_begin, new_it_vec_end, it, sits_count, tmp, prev_visited_to_save };
                    args.push(new_arg);
                    continue;
                }//end else of can go FORWARD by the reason of it!=pnodes.end()
            }//end of if( it_vec_begin!=it_vec_end  && (sits_count<=sits_max) )
        }//end while(true)

        return result == INT_MAX ? -1 : result;
    }
};

int main()//for testing
{ 
    int start = 1, finish =6;
    int stops_max =2;
    int flights_number = 4;//not used in algorithm, but present to fit the given by leetcode signature 
    
        std::vector< std::vector<int> > flights
        {//from, to, price
        {1, 2, 100 }, {1, 3, 200}, {1, 5, 5}, {2, 3, 5},
        {2, 5, 50}, {2, 6, 30}, {3, 5, 2}, {5, 6, 10}, {5, 1, 1000}
        }; 

    int res = Solution{}.findCheapestPrice(flights_number, flights, start, finish, stops_max);
    std::cout << "\nmy result= " << res;//15 -> 1-5-6 = 5+10
    return 0;
}
