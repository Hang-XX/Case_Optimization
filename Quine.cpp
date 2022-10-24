#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <numeric>
#include <queue>
#include <list>
#include "Quine.h"
using namespace Quine;
Node::Node(  string & term , int level){
    this->level = level;
    this->term = term;
    this->covered = false;
    printf("Object Node is being created... ... \n");
}

int Node::one_num()
{
    int num = count(this->term.begin(),this->term.end(),'1');
    return num;
}

string Node::getterm(){
    return this->term;
}

int* Node::compare(Node node1)
{
    int *cmp = new int[2];
    std::vector<int> res;
    for (int i = 0; i < this->term.length() ; i++)
    {
        if(this->term[i] == node1.term[i])
        {
            continue;
        }
        else if (this->term[i] == '-' || node1.term[i] == '-')
        {
            cmp[0] = 0;
            cmp[1] = 0; 
            return cmp;
        }
        else
        {
            res.push_back(i);
        }
    }

    if(res.size() == 1)
    {
        cmp[0] = 1;
        cmp[1] = res[0];
        return cmp;
    }
    cmp[0] = 0;
    cmp[1] = 0;
    return cmp;
}

std::string Node::term2logic()
{
    string logic_tmp;
    for(int i = 0; i < this->term.size(); i++)
    {
        if(this->term[i] == '-')
        {
            continue;
        }
        else if (this->term[i] == '1')
        {
            logic_tmp += "A";
            logic_tmp += std::to_string(i);
        }
        else
        {
            logic_tmp += "A";
            logic_tmp += std::to_string(i);
            logic_tmp += "\'";
        }
    }
    if(logic_tmp.size() == 0)
    {
        logic_tmp = "1";
    }
    return logic_tmp;
}

Group::Group(int num , std::vector<int> & lst){
    std::vector<std::vector<Node>> group_list(num);
    this->max_bits = num;
    this->min_list =  lst;
    if(this->min_list.size() == 0)
    {
        printf("The input array is empty! \n");
    }
    if(this->min_list.back() >= pow(2,this->max_bits))
    {
        printf("The input value error\n");
    }
    // cout<<"min_list"<<endl;
    // for(vector<int>::iterator it=this->min_list.begin();it != this->min_list.end();it++)
    // {
    //     cout << *it << ","<<endl;
    // }
}
int Group::spilt_group(){
    std::vector<std::vector<Node>  > group;
    int flag = 1;
    for (int count = 0 ; count <= this->max_bits; count++)
    {
        std::vector<Node> subgroup;
        for(vector<int>::iterator it = this->min_list.begin();it != this->min_list.end(); it++)
        {
            auto term_tmp = this->num2bitstr(*it);
            Node node_tmp(term_tmp, 0);
            auto one_num = node_tmp.one_num();
            if(one_num == count ){
                subgroup.push_back(node_tmp);
                cout<<"subgroup"<<one_num << ":Node term"<< node_tmp.getterm()<<endl;
            }
            // this->node_list[node_tmp.one_num()].push_back(node_tmp);
        }
        group.push_back(subgroup);
    }
    this->node_list.push_back(group);
// Debug
    for (int i =0; i < group.size(); i ++)
    {
        for (int j =0 ; j < group[i].size(); j++)
        {
            cout<<"init group "<<"["<<i<<"]"<<"["<<j<<"]"<<":"<<group[i][j].getterm()<<endl;
        }
    }
    return flag;
}

// std::string Group::num2bitstr(int num){
//     string r;
//     while (num != 0 ){
//         r+= ( num % 2 == 0 ? "0" : "1" );
//         num /= 2;
//     }
//     reverse(r.begin(),r.end());
//     int rel = this->max_bits - r.length();
//     // while(rel > 0)
//     // {
//     //     r = "0" + r;
//      //    rel--;
//     // }
//     return r;
// }
std::string Group::num2bitstr(int num){
    string r = "";
    while (num != 0 ){
        if(num % 2 == 0) r = '0' + r;
        else r = '1' + r;
        num /= 2;
    }
    int rel = this->max_bits - r.length();
    if(rel < 0)
    {
        throw "The input value overflows! \n";
    }
    while(rel > 0)
    {
        r = "0" + r;
        rel--;
    }
    return r;
}

void Group::merge(int level){
    int flag = 0;
    if (level == 0){
        flag=this->spilt_group();
    }
    else
    {
        printf("level > 0 \n");
        auto &groups = this->node_list[level -1];
        std::vector<std::vector<Node> > next_groups;
        unordered_set<string> term_set;
        for (int count = 0 ; count <= this->max_bits; count++)
        {
            std::vector<Node> subgroup;
            for (int i = 0; i < groups.size()-1 ; i++)
            {
                for (auto &node0 : groups[i])
                {
                    for(auto &node1 : groups[i+1])
                    {
                       int* cmp = node0.compare(node1);
                       if(cmp[0])
                       {
                           node0.covered = true;
                           node1.covered = true;
                           auto new_term = node0.term;
                           new_term.replace(cmp[1],1,"-");
                           printf("node0 term :%s \n",node0.term.c_str());
                           printf("node1 term :%s \n", node1.term.c_str());
                           printf("是否可以合并:%d \n",cmp[0]);
                           printf("合并的位置: %d \n",cmp[1]);
                           printf("new_term :%s \n", new_term.c_str());
                           auto tmp_ndoe = Node(new_term , level);
                           if (term_set.find(tmp_ndoe.term) == term_set.end())
                           {
                                if(tmp_ndoe.one_num() == count)
                                {
                                    subgroup.push_back(tmp_ndoe);
                                    term_set.insert(tmp_ndoe.term);
                                    flag = 1;
                                }
                           }
                       }
                       delete[] cmp;
                    }
                }
            }
            next_groups.push_back(subgroup);
        }
        this->node_list.push_back(next_groups);    
    }
    if(flag){
        this->merge(level + 1);
    }
    //Debug
    for (int z = 0 ; z < this->node_list.size(); z++)
    {
        for (int i =0; i < this->node_list[z].size(); i ++)
        {
            for (int j =0 ; j < this->node_list[z][i].size(); j++)
            {
                cout<<"node_list "<<"["<<z<<"]"<<"["<<i<<"]"<<"["<<j<<"]"<<":"<<this->node_list[z][i][j].getterm()<<endl;
                // cout<<"node_list "<<"["<<z<<"]"<<"["<<i<<"]"<<"["<<j<<"]"<<"covered"<<":"<<this->node_list[z][i][j].covered<<endl;
            }
        }
    }
}

void Group::Set_RIList()
{
     for (int z = 0 ; z < this->node_list.size(); z++)
    {
        for (int i =0; i < this->node_list[z].size(); i ++)
        {
            for (int j =0 ; j < this->node_list[z][i].size(); j++)
            {
                auto tmp_node = this->node_list[z][i][j];
                if(tmp_node.covered == false)
                {
                    this->PI.push_back(tmp_node);
                    printf("PI List : %s \n",tmp_node.term.c_str());
                }
            }
        }
    }
}

void  Group::cover_left(std::vector<std::vector<int> >& chart , std::vector<std::vector<int> >& list_result)
{
    // std::vector<std::vector<int> > list_result;
    auto max_len = chart.size();
    cout<<"max_len: "<<max_len<<endl;
    queue<list<int> > myQ;
    for (int i = 0; i < max_len ; i++)
    {
        myQ.push({i});
    }
    // cout<<"myQ size: "<<myQ.size()<<endl;
    int stop_flag = false;
    while(~myQ.empty())
    {
        std::vector<int> min_mark ;
        auto choice = myQ.front();
        list<int>::iterator i_tmp;
        i_tmp =choice.begin();
        for(int i = 0; i< choice.size(); i++)
        {
            // cout<<"choice size :"<<choice.size()<<endl;
            cout<<"choice "<< i<<":"<<*i_tmp<<endl;
            i_tmp++;
        }
        myQ.pop();
        // cout<<"after pop myQ size"<<myQ.size()<<endl;
        if(stop_flag && (list_result.back().size() < choice.size()))
        {
            printf("sign out !\n");
            break;
        }
        for (int i = 0; i<chart[*choice.begin()].size(); i++) // col
        {   list<int>::iterator iter;
            int tmp = 0;
            // cout<<"chart[0] size"<< chart[*choice.begin()].size()<<endl;
            iter=choice.begin();
            for(int j = 0; j < choice.size(); j++) //row
            {
                // cout<<"chart size: "<<choice.size()<<endl;
                // cout<<"chart"<<*iter<<","<<i<<chart[*iter][i]<<endl;
                tmp = chart[*iter][i] + tmp;
                // cout<< "tmp:"<< tmp<<endl;
                iter = iter++;
            }
            min_mark.push_back(tmp);
            cout<<"min mark"<<i<<":"<<tmp<<endl;
        }
        if(count(min_mark.begin(),min_mark.end(), 0) == 0)
        {
            std::vector<int> choice_trans;
            for(auto iterm : choice)
            {
                choice_trans.push_back(iterm);
            }
            list_result.push_back(choice_trans);
            for(int i = 0; i<choice_trans.size();i++)
            {
                cout<<"choice trans "<<i<<":"<<choice_trans[i]<<endl;
            }
            stop_flag = true;     //不直接退出
        }
        for(int i = choice.back()+1; i < max_len; i++)
        {   
            auto n_choice = choice;
            n_choice.push_back(i);
            myQ.push(n_choice);
            list<int>::iterator i_tmp;
            i_tmp =n_choice.begin();
            cout<<"产生新节点"<<endl;
            for(int i=0;i<n_choice.size();i++)
            {
                cout<<"节点的第"<<i<<"维的数值:"<<*i_tmp<<endl;
                i_tmp++;
            }
        }
    }

    // return list_result;
}
void Group::Final_out()
{
    std::vector<int> chart_col(this->min_list.size(),0);
    std::vector<std::vector<int> > chart(this->PI.size(),chart_col);
    for (int i = 0; i < this->PI.size() ; i++)
    {
        for (int j = 0; j < this->min_list.size(); j++)
        {
            if (this->Cmp_bin_same(this->PI[i].term, this->num2bitstr(this->min_list[j]) ) )
            {
                chart[i][j]  = 1;
                cout<< "chart"<<"["<<i<<"]"<<"["<<j<<"] :"<<chart[i][j]<<endl;
            }else
            {
                chart[i][j] = 0;
                cout<< "chart"<<"["<<i<<"]"<<"["<<j<<"] :"<<chart[i][j]<<endl;
            }
        }
    }
    std::vector<std::vector<int> >prime;
    this->find_min_cost(chart,prime);

    for(auto pri : prime)
    {
        std::string str;
        for (int i = 0; i< this->PI.size(); i++)
        {
            cout<<"PI size: "<<this->PI.size()<<endl;
            for(int j = 0; j < pri.size(); j++)
            {   
                // auto tmp = pri[j];
                
                if(i == pri[j])
                {
                    str = str + this->PI[i].term2logic() + '+';
                    cout<<"PI"<<i<<":"<<this->PI[i].term<<endl;
                }
            } 
        }
        if(str.back() == '+')
        {
            str = str.substr(0,str.length()-1);
        }
        printf("final out : %s \n",str.c_str());
    }
}

int Group::Cmp_bin_same(const std::string& term,const std::string& num)
{
    for(int i = 0; i < term.size() ; i++)
    {
        if (term[i] != '-')
        {
            if (term[i] != num[i])
                return false;
        }
    }
    return true;
}
        
void Group::find_min_cost(std::vector<std::vector<int> >& chart,std::vector<std::vector<int> >&QM_final )
{
    std::vector<int> essential_prime;
    this->find_essential_prime(chart,essential_prime);
    //update chart
    for(int i = 0 ; i < essential_prime.size(); i++)
    {
        for(int j = 0; j < chart[0].size(); j++)
        {
            if(chart[essential_prime[i]][j] == 1)
            {
                for(int z = 0; z < chart.size(); z++)
                {
                    chart[z][j] = 0;
                }
            }
        }
    }
    if(accumulate(essential_prime.begin(),essential_prime.end(),0) == 0)
    {
        QM_final.push_back(essential_prime);
    }
    else
    {
        std::vector<int> chart_left_col;
        std::vector<int> chart_left_row;
        for (int i = 0 ; i < chart[0].size(); i++)   //col
        {   
            int tmp_sum = 0;
            for(int j = 0; j < chart.size(); j++)   //row
            {
                tmp_sum += chart[j][i];
            }
            cout<<"chart_left的"<<"第"<<i<<"列"<<"sum为"<<tmp_sum<<endl;
            if(tmp_sum > 0)
            {
                chart_left_col.push_back(i);
            }
        }
        cout<<"chart size"<<chart.size()<<endl;
        for (int i = 0 ; i < chart.size(); i++)   //row
        {   
            int tmp_sum = 0;
            for(int j = 0; j < chart[i].size(); j++)   //col
            {
                tmp_sum += chart[i][j];
            }
            cout<<"chart_left的"<<"第"<<i<<"行"<<"sum为"<<tmp_sum<<endl;
            if(tmp_sum > 0)
            {
                chart_left_row.push_back(i);
            }
        }
        std::vector<int> new_chart_col(chart_left_col.size(),0);
        std::vector<std::vector<int> > new_chart(chart_left_row.size(),new_chart_col);
        cout<<"new chart size "<<chart_left_row.size()<< ","<<chart_left_col.size()<<endl;
        cout<<"new chart size real "<<new_chart.size()<<","<<new_chart[0].size()<<endl;
        for(int i = 0; i < chart_left_row.size(); i ++)
        {
            for(int j = 0; j < chart_left_col.size(); j++)
            {
                if(chart[chart_left_row[i]][chart_left_col[j]] == 1)
                {
                    new_chart[i][j] = 1;
                    cout<<"new chart "<<i<<","<<j<<":"<<new_chart[i][j]<<endl;
                }
            }
        }
        std::vector<std::vector<int> > list_result;
        this->cover_left(new_chart,list_result);
        //这里可能存在list_result有多种可行的情况
        for(auto& lst_tmp:list_result)
        {
            auto essen_prime_tmp = essential_prime;
            for(auto x :lst_tmp)
            {
                essen_prime_tmp.push_back(chart_left_row[x]);
            }
            QM_final.push_back(essen_prime_tmp);
        }
    }
    for(int i = 0; i< QM_final.size();i++)
    {   for(int j = 0; j<QM_final[i].size(); j++)
            {
                cout<<"QM finial "<<i<<","<<j<<":"<<QM_final[i][j]<<endl;
            }
    }
}

void Group::find_essential_prime(std::vector<std::vector<int> >& chart,std::vector<int>& essential_prime )
{
    std::vector<int> chart_sum_p;
    for (int i = 0 ; i < chart[0].size(); i++)   //col
    {   
        // printf("chart[0] size:%d \n",chart[0].size());
        int tmp_sum = 0;
        for(int j = 0; j < chart.size(); j++)   //row
        {
            tmp_sum += chart[j][i];
        }
        cout<<"chart的"<<"第"<<i<<"列"<<"sum为"<<tmp_sum<<endl;
        if(tmp_sum==1)
        {
            chart_sum_p.push_back(i);
        }
        // printf("chart_sum_p",chart_sum_p.back());
    }
    // std::vector<int> essential_prime;
    for(int i = 0; i<this->PI.size(); i++)  //row
    {
        for (int j = 0; j < chart_sum_p.size(); j++) 
        {
            if(chart[i][chart_sum_p[j]] == 1)
            {
                essential_prime.push_back(i);
            }
        }
    }
    // 删除重复项
    sort(essential_prime.begin(),essential_prime.end());
    essential_prime.erase(unique(essential_prime.begin(),essential_prime.end()),essential_prime.end());
}




