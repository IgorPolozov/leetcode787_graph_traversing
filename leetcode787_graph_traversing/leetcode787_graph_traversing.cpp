#include <vector>
#include <stack>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

namespace sol_simple_struct{

class Solution//leetcode 787 task: https://leetcode.com/problems/cheapest-flights-within-k-stops/description/
{
public:    
    using vec_t=std::vector<int>;
    using pr_t=std::pair<int,int>;
    using vec_pr_t=std::vector<pr_t>;
    using smrt_vec_pr_t=std::vector<vec_pr_t>;

    bool is_vised_by(const vec_t &visitors, int never_city, int visitee, int visitor)const//this, other
        {
            bool result=false;

            int current_visitor=visitor;

            while(current_visitor!=never_city)
            {
                if(current_visitor==visitee)result=true;
                current_visitor=visitors[current_visitor];
            }
            return result;
        }

int findCheapestPrice(const int& n, const std::vector<vec_t>& flights, const int& src, const int& dst, const int& k)
    {
        int src_ind=src, dst_ind=dst;

        std::vector<vec_t> fls(flights);

        auto lmbd_compare=[](const vec_t &a, const vec_t &b){return a[0]<b[0];};

        std::unordered_map<int, int, std::hash<int>> cities_by_condition(n);

//create map for reindex cities from
        int ind=0;
        for(const auto &subv:fls)
        {
            if(cities_by_condition.insert({subv[0],  ind}).second) ++ind;
        }
        for(const auto &subv:fls)
        {
            if(cities_by_condition.insert({subv[1],  ind}).second) ++ind;
        }

        if(cities_by_condition.count(src)==0)return -1;
        if(cities_by_condition.count(dst)==0)return -1;

        for(auto &el:cities_by_condition)
        {
           if(el.first==src)src_ind=el.second;
           if(el.first==dst)dst_ind=el.second;
        }        

        for (auto &vec: fls)
        {
            vec[0]=cities_by_condition[vec[0]];
            vec[1]=cities_by_condition[vec[1]];
        }      

        cities_by_condition.clear();

        std::sort(fls.begin(), fls.end(), lmbd_compare);              

        const int never_city=-1;//to avoid dependense on the cities number
        std::vector<int> visitors(n, never_city);

        smrt_vec_pr_t smart_fls;
        smart_fls.reserve(n);

        auto smrt_vit=fls.begin();
        auto smrt_vit_end=fls.end();
        while(smrt_vit!=smrt_vit_end)
        {
            auto eq_rng_pr=std::equal_range(smrt_vit, smrt_vit_end, vec_t{(*smrt_vit)[0]}, lmbd_compare);
            vec_pr_t tmp_vec_t;
            tmp_vec_t.reserve(eq_rng_pr.second-eq_rng_pr.first);
            pr_t pair_ints;
            while(eq_rng_pr.first!=eq_rng_pr.second)
            {
                pair_ints.first=(*eq_rng_pr.first)[1];//city to
                pair_ints.second=(*eq_rng_pr.first)[2];//price from - to
                tmp_vec_t.push_back(pair_ints);
                ++eq_rng_pr.first;
            }
            smart_fls.push_back(tmp_vec_t);
            if(eq_rng_pr.second==smrt_vit_end)break;
            smrt_vit=eq_rng_pr.second;
        }

        fls.clear();

        for(auto &sbv:smart_fls)
        std::sort(sbv.begin(), sbv.end(), [](const pr_t &a, const pr_t &b){return a.second<b.second;});

      int result=INT_MAX, price=0, tmp=0, sits_count=0;

     struct arg
            {
                int city_from;//curemt city from which we go to the next one, initially=src;
                int tmp;//accumulates summ of all previous prices
                int previously_visited_by;//initially=never_city;
                vec_pr_t::iterator it_vec_begin;//iterates among candidates to be the next one
            };

            arg init_arg{src_ind, tmp, never_city, smart_fls[src_ind].begin()};

            std::stack<arg> args;//city id, previous visitor id
            args.push(init_arg);

        while(true)
        {           
                   while( !args.empty() && (args.top().it_vec_begin==smart_fls[args.top().city_from].end()) )//pop off all inaccessible ones
                   {
                       int city_from_to_pop= args.top().city_from;
                       visitors[city_from_to_pop]=args.top().previously_visited_by;
                       args.pop();
                        --sits_count;
                   }

                    if(args.empty()) break;

                    auto &args_obj=args.top();

                    auto it_vec_begin=args_obj.it_vec_begin;
                    price=it_vec_begin->second;
                    tmp=args_obj.tmp;

                     if(
                      (  (tmp+price)  >  result  )
                      ||
                      (sits_count>k)//k is sits_max
                      )
                       {
                         ++args.top().it_vec_begin;//go right
                                continue;
                       }

                       if(args.top().city_from==src_ind){
                         visitors=vec_t(n, never_city);//move operation =
                         tmp=0;
                            sits_count=0;
                     }

                     bool visited=false;

                     visited=is_vised_by(visitors, never_city, it_vec_begin->first, args.top().city_from);

            if(!visited)
            visited=is_vised_by(visitors, never_city, args.top().city_from, it_vec_begin->first);

            if( visited ){
                       ++args.top().it_vec_begin;
                        continue;
             }

            if(it_vec_begin->first==dst_ind){
                int probably=tmp+price;
                result=probably<result?probably:result;
                ++args.top().it_vec_begin;//go right to find longer but chipper variant
                continue;
           }

             int prev_visited_to_save=visitors[it_vec_begin->first];
                    visitors[it_vec_begin->first]=args.top().city_from;
                        int new_from=it_vec_begin->first;
                        auto it_vec_begin_new=smart_fls[new_from].begin();
                            ++args.top().it_vec_begin;
           ++sits_count;//here only
                   tmp+=price;//here only

           arg new_arg{new_from, tmp, prev_visited_to_save, it_vec_begin_new};
           args.push(new_arg);
        }
    return result==INT_MAX?-1:result;
    }
};
