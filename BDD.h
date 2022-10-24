#include <vector>
#include <iostream>
#ifndef BDD_H
#define BDD_H

namespace BDD
{

// std::vector<std::vector<int>> addrMatix;

// std::vector<std::vector<std::string>> output_Node;
/**
 * BDD Node
*/
class Node 
{
    public:
    //node type = "MUX node"
    Node(std::string& low,std::string& high,std::string& sel,std::string& out,std::string& node_type){
        this->low = low;
        this->high = high;
        this->sel = sel;
        this->out = out;
        this->Node_type = node_type;
        std::cout<< " Node type is : "<<this->Node_type<<std::endl;
        std::cout<< " Node input low  is : "<<this->low<<std::endl;
        std::cout<< " Node input high  is : "<<this->high<<std::endl;
        std::cout<< " Node  sel  is : "<<this->sel<<std::endl;
        std::cout<< " Node output is : "<<this->out<<std::endl;
    }
    //node type == "AND node"
    Node(std::string& low,std::string& high,std::string& out,std::string& node_type){
        this->low = low;
        this->high = high;
        this->sel = "None";
        this->out = out;
        this->Node_type = node_type;
    }
    // node type == "NOT Node"
    Node(std::string& input, std::string& out ,std::string& node_type)
    {
        this->low = input;
        this->high = "None";
        this->sel = "None";
        this->out = out;
        this->Node_type = node_type;
        std::cout<< " Node type is : "<<this->Node_type<<std::endl;
        std::cout<< " Node input is : "<<this->low<<std::endl;
        std::cout<< " Node output is : "<<this->out<<std::endl;
    }
    
    int Node_level_r;
    int Node_level_c;
    std::string get_nodetype(){return this->Node_type;}
    std::string get_low(){return this->low;}
    std::string get_high(){return this->high;}
    std::string get_sel(){return this->sel;}
    std::string get_out(){return this->out;}
    std::string Node_type;

//    private:

    //input
    std::string low;
    std::string high;
    //select 
    std::string sel;

    float sel_delay = 0;

    // std::string sel_attrs;
    //output
    std::string out;
};

class Node_info
{
    public:
    // tmp row group
    std::vector<BDD::Node> row_MUXgroup;

    // tmp datamat and  addrmat
    std::vector<std::string> data_s;
    std::vector<std::vector<int> > addr_i;
    
    //tmp delay info
    std::vector<float > delay_tmp;

    //Used to mark the last 
    int flag_last = 0;

    std::vector<int> addr_left;

    std::string sel_s;

    std::vector<std::string> sel_var;


    std::string var_out;

    int addrmat_r;

};

 /** BDD
  * This is the intermediate representation for optimization pass
 */
class Tree
{
    public:
    Tree(int tree_level_r,int tree_level_c, int Sel_level_r,int Sel_level_c,std::vector<std::vector<int> >& addrmat,int tree_num){
        this->Tree_level_r = tree_level_r;
        this->Tree_level_c = tree_level_c;
        this->Sel_level_r = Sel_level_r;
        this->Sel_level_c = Sel_level_c;
        this->addrmat = addrmat;
        this->addrmat_size_c = addrmat[0].size();
        this->delay.resize(addrmat.size()+1);
        this->tree_num = tree_num;
    }
    int Tree_level_r;
    int Tree_level_c;
    int Sel_level_r;
    int Sel_level_c;
    int tree_num;

    // Temporary addrmat and datamat
    std::vector<std::vector<int> > addrmat;
    std::vector<std::string> datamat_s;

    // Node groups
    std::vector<std::vector<BDD::Node> > groups;
    std::vector<BDD::Node > sel_groups;

    //Statistics of delay
    std::vector<float> delay;

    int get_size_c(){return this->addrmat_size_c;}
  
    private: 
    int addrmat_size_c;


};

void pre_create(std::vector<int >& datamat,Tree& tree );

void create_groups(Tree& tree);

void create_bdd(std::vector<std::vector<int> >& addrmat, std::vector<int >& datamat,Tree& Bdd_tree);

void Mux_node0(Tree& Bdd_tree,Node_info& node_info,int& i);

void Mux_node1(Tree& Bdd_tree,Node_info& node_info,int& i);

void Mux_node2(Tree& Bdd_tree,Node_info& node_info,int& i);

void Mux_node3(Tree& Bdd_tree,Node_info& node_info,int& i);

void visual_tree(Tree& Bdd_tree);

float max_data(float& a,float& b);

void row_group_merge(Tree& Bdd_tree,Node_info& node_info,std::string& data_left,std::string& data_right,float& delay_left,float& delay_right,bool& row_node_tmp);
}

#endif              // BDD_H