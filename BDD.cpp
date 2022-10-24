#include <assert.h>
#include "BDD.h"

#define NOT_Delay       0.1
#define MUX_Delay       2
#define AND_Delay       1

void BDD::create_bdd(std::vector<std::vector<int> >& addrmat, std::vector<int>& datamat,Tree& Bdd_tree)
{
    //addrmat_c have same size,这里的输入要求addrmat的列相等
    BDD::pre_create(datamat, Bdd_tree);
    // for(int i = 0; i < Bdd_tree.groups[0].size() ; i++)
    // {
    //     std::cout <<"Bdd NOT group : "<<Bdd_tree.groups[0][i].Node_type<<std::endl;
    // }

    for(int row_num = 0; row_num < addrmat[0].size();row_num++)
    {
    //     std::cout<<"groups["<<Bdd_tree.addrmat[0].size()<<"]"<<"is being created!"<<std::endl;
        BDD::create_groups(Bdd_tree);

    }

    
}

void BDD::pre_create( std::vector<int >& datamat,BDD::Tree& tree)
{
    int datamat_r = datamat.size();
    std::vector<BDD::Node> row_group;

    for (int i = 0 ; i<datamat_r ; i++)
    {
        std::string data_int_s;
        std::string var_out;
        auto data_int = datamat[i];
        if(data_int == 0)
        {
            data_int_s = "1'b0";
            var_out = data_int_s;
            std::cout<<" var out: "<<var_out<<std::endl;
            tree.datamat_s.push_back(var_out); 

        }else if(data_int == 1)
        {
            data_int_s = "1'b1";
            var_out = data_int_s;
            std::cout<<" var out: "<<var_out<<std::endl;
            tree.datamat_s.push_back(var_out); 

        }else if(data_int%2 == 0)
        {
            data_int_s = "din[" + std::to_string((data_int-2)/2) + ']';
            var_out = data_int_s;
            std::cout<<" var out: "<<var_out<<std::endl;
            tree.datamat_s.push_back(var_out); 

        }else
        {
            //create NOT node
            std::string var_tmp;
            std::cout<< "Creating NOT Node ....................."<<std::endl;
            data_int_s = "din[" + std::to_string((data_int-3)/2) + ']';
            var_tmp = "n"+ std::to_string(tree.tree_num)+std::to_string(tree.Tree_level_r) + std::to_string(tree.Tree_level_c);
            std::string node_type = "NOT Node";

            if(row_group.size() > 0)
            {
                bool flag_tmp = false;
                for (int j = 0 ; j < row_group.size(); j++ )
                {
                    if(row_group[j].get_low() == data_int_s)
                    {
                        std::cout<<"Duplicate nodes do not need to be created !"<<std::endl;
                        var_out = row_group[j].get_out();
                        flag_tmp = true;
                    }
                }
                if(flag_tmp == false)
                {
                    std::cout<<"new node is being created !"<<std::endl;
                    var_out = var_tmp;
                    BDD::Node node_tmp(data_int_s,var_out,node_type);
                    node_tmp.Node_level_c = tree.Tree_level_c;
                    node_tmp.Node_level_r = tree.Tree_level_r;
                    row_group.push_back(node_tmp);
                    tree.Tree_level_c ++;

                    //Accumulate delay information
                    tree.delay[i] = tree.delay[i] + NOT_Delay;
                }
            }else
            {
                std::cout<<"first node is being created !"<<std::endl;
                var_out = var_tmp;
                BDD::Node node_tmp(data_int_s,var_out,node_type);
                node_tmp.Node_level_c = tree.Tree_level_c;
                node_tmp.Node_level_r = tree.Tree_level_r;
                row_group.push_back(node_tmp);
                tree.Tree_level_c ++;
                
                //Accumulate delay information
                tree.delay[i] = tree.delay[i] + NOT_Delay;

            }
            tree.datamat_s.push_back(var_out); 

        }
    }
  
    tree.Tree_level_r ++;
    tree.Tree_level_c = 0;
    std::cout<<"tree datamat_s size is :"<<tree.datamat_s.size()<<std::endl;
    for (int i = 0 ; i<tree.datamat_s.size(); i++)
    {
        std::cout<<"tree datamat_s["<<i<<"]: "<<tree.datamat_s[i]<<std::endl;
        std::cout<<"tree delay["<<i<<"]: "<<tree.delay[i]<<std::endl;

    }
    // std::cout<<"row group size is: "<<row_group.size()<<std::endl;
    // for (int j = 0 ; j<row_group.size(); j++)
    // {
    //     std::cout<<"row group Node type "<<row_group[j].Node_type<<std::endl;
    // }
    for(int i = 0; i<tree.addrmat.size(); i++ )
    {
        for(int j = 0; j<tree.addrmat[i].size();j++)
        {
            std::cout<<"tree addrmat ["<<i<<"]["<<j<<"]: "<<tree.addrmat[i][j]<<std::endl;
        }
    }
    tree.groups.push_back(row_group);
    row_group.clear();
}


void BDD::create_groups(Tree& Bdd_tree)
{
    BDD::Node_info node_info;
    int addrmat_r = Bdd_tree.addrmat.size();
    node_info.addrmat_r = addrmat_r;
    std::cout << "addrmat row:"<<addrmat_r<<std::endl;
    int addrmat_c = Bdd_tree.addrmat[0].size();
    std::cout << "addrmat col:"<<addrmat_c<<std::endl;

    std::cout<< "Bdd tree sel level c : "<<Bdd_tree.Sel_level_c<<std::endl;
    std::cout<<" Bdd tree sel level r : "<<Bdd_tree.Sel_level_r<<std::endl;

    for (int i = 0 ; i < addrmat_r -1 ; i++)
    {
        // std::string var_out;
        auto addr_left = Bdd_tree.addrmat[i]; 
        auto addr_right = Bdd_tree.addrmat[i+1]; 
	    assert(addr_left.size() == addr_right.size());
        node_info.addr_left = addr_left;
        bool high_eq = true;
        for (int j = 0 ; j < addr_left.size()-1; j++)
        {
            int addr_left_h = addr_left[j];
            int addr_right_h = addr_right[j];
            if(addr_right_h ==  -1 || addr_left_h == -1)  //这里需要考虑一下,输入全部展开就不需要
            {
                continue;
            }else if (addr_left_h == addr_right_h)
            {
                continue;
            }
            else
            {
                high_eq = false;
            }
        }

        int addr_left_l = addr_left.back();
        int addr_right_l = addr_right.back();

        node_info.sel_s = "sel[" + std::to_string(Bdd_tree.get_size_c()-addrmat_c) +"]";

        //Whether the last elements are equal;
        //If they are not equal, they may be the same node
        bool low_nq = (addr_left_l ==0 && addr_right_l == 1); 
        // There are four scenarios to consider
        if(high_eq == true && low_nq == true)
        {

            BDD::Mux_node0(Bdd_tree,node_info,i);

        }else if(addr_left_l == -1)  //Nodes are not created when selector inputs are equal
        {
            node_info.var_out = Bdd_tree.datamat_s[i];
            auto delay = Bdd_tree.delay[i];
            std::vector<int> addr_h(addr_left.begin(),addr_left.end()-1);
            node_info.data_s.push_back(node_info.var_out);
            node_info.addr_i.push_back(addr_h);

            //Delay info
            
            node_info.delay_tmp.push_back(delay);

        }else if (addr_left_l == 0  )
        {
            BDD::Mux_node1(Bdd_tree,node_info,i);

        }else if(addr_left_l == 1)
        {
            BDD::Mux_node2(Bdd_tree,node_info,i);
        }
        std::cout<<"flag last :"<<node_info.flag_last <<std::endl;
        std::cout<<"i :"<<i<<std::endl;
        if( i == addrmat_r -2 && node_info.flag_last == 0)
        {
            BDD::Mux_node3(Bdd_tree,node_info,i);
        }

    }
    Bdd_tree.groups.push_back(node_info.row_MUXgroup);
    node_info.data_s.push_back(Bdd_tree.datamat_s.back());
    node_info.delay_tmp.push_back(Bdd_tree.delay.back());
    node_info.row_MUXgroup.clear();
    Bdd_tree.datamat_s.clear();
    Bdd_tree.addrmat.clear();
    Bdd_tree.delay.clear();
    Bdd_tree.datamat_s = node_info.data_s;
    Bdd_tree.addrmat = node_info.addr_i;
    if(node_info.delay_tmp.size() > 1)
    {
        Bdd_tree.delay = node_info.delay_tmp;
    }
    Bdd_tree.Tree_level_r ++;
    Bdd_tree.Tree_level_c = 0;
    Bdd_tree.Sel_level_r ++;
    Bdd_tree.Sel_level_c = 0;
    std::cout<<"Bdd_tree datamat_s size is :"<<Bdd_tree.datamat_s.size()<<std::endl;
    for (int i = 0 ; i<Bdd_tree.datamat_s.size(); i++)
    {
        std::cout<<"Bdd_tree datamat_s["<<i<<"]: "<<Bdd_tree.datamat_s[i]<<std::endl;
        std::cout<<"Bdd_tree delay["<<i<<"]: "<<Bdd_tree.delay[i]<<std::endl;

    }
    std::cout<<"Bdd_tree addrmat size is: "<<node_info.row_MUXgroup.size()<<std::endl;
    for (int j = 0; j<Bdd_tree.addrmat.size(); j++)
    { 
        for(int z = 0; z<Bdd_tree.addrmat[j].size(); z++)
        {
            std::cout<<"Bdd_tree addrmat ["<<j<<"]["<<z<<"]:"<<Bdd_tree.addrmat[j][z]<<std::endl;
        }
    }
}

void BDD::Mux_node0(Tree& Bdd_tree,Node_info& node_info,int& i)
{
    auto data_left = Bdd_tree.datamat_s[i];
    auto data_right = Bdd_tree.datamat_s[i+1];
    auto delay_left = Bdd_tree.delay[i];
    auto delay_right = Bdd_tree.delay[i+1];
    // No new node is created if two inputs to the node are the same
    if(data_left != data_right)
    {
        std::cout<<"addrmat["<<i<<"] and addrmat["<<i+1<<"] can be merged into a single node"<<std::endl;
        bool row_node_tmp = false; 
        row_group_merge(Bdd_tree,node_info,data_left,data_right,delay_left,delay_right,row_node_tmp);
        if(node_info.row_MUXgroup.size() >0 && row_node_tmp == false)
        {    bool flag_tmp = false;
            for(int z = 0; z<node_info.row_MUXgroup.size(); z++)
            {
                if(node_info.row_MUXgroup[z].get_low() == data_left && node_info.row_MUXgroup[z].get_high() == data_right)
                {
                    std::cout <<"The current node already exists !"<<std::endl;
                    std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                    node_info.data_s.push_back(node_info.row_MUXgroup[z].get_out()); 
                    node_info.addr_i.push_back(addr_h);
                    flag_tmp = true;
                    //Delay info
                    // auto max_delay = BDD::max_data(delay_left,delay_right);
                    auto max_delay = node_info.delay_tmp[z];
                    node_info.delay_tmp.push_back(max_delay);
                }
            }
            if(flag_tmp == false)
            {
                node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                std::cout<<"new MUX node1 is being created !"<<std::endl;
                std::string node_type = "MUX Node"; 
                //create MUX node
                BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                node_info.row_MUXgroup.push_back(node_MUX);
                Bdd_tree.Tree_level_c ++;
                node_info.data_s.push_back(node_info.var_out); 
                node_info.addr_i.push_back(addr_h);
                //Delay info
                auto max_delay = BDD::max_data(delay_left,delay_right);
                max_delay = max_delay + MUX_Delay;
                node_info.delay_tmp.push_back(max_delay);
            }
        }else if( row_node_tmp == false){
            node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
            std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
            std::cout<<"first MUX node1 is being created !"<<std::endl;
            std::string node_type = "MUX Node"; 
            //create MUX node
            BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
            node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
            node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
            node_info.row_MUXgroup.push_back(node_MUX);
            Bdd_tree.Tree_level_c ++;
            node_info.data_s.push_back(node_info.var_out); 
            node_info.addr_i.push_back(addr_h);
            //Delay info
            auto max_delay = BDD::max_data(delay_left,delay_right);
            max_delay = max_delay + MUX_Delay;
            node_info.delay_tmp.push_back(max_delay);
        }
    }else{
        node_info.var_out = Bdd_tree.datamat_s[i];
        std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
        node_info.data_s.push_back(node_info.var_out);
        node_info.addr_i.push_back(addr_h);
        
        //Delay info
        auto max_delay = BDD::max_data(delay_left,delay_right);
        node_info.delay_tmp.push_back(max_delay);
    }
    // if(i < node_info.addrmat_r -3)
    // {
    // }
    if(i ==  node_info.addrmat_r -2)
    {
        node_info.flag_last = 1;
    }
    i++; // skip two nodes
}

void BDD::Mux_node1(Tree& Bdd_tree,Node_info& node_info,int& i)
{
    auto data_left = Bdd_tree.datamat_s[i];
    auto delay_left = Bdd_tree.delay[i];
    auto data_right = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-1];
    auto delay_right = Bdd_tree.delay[Bdd_tree.datamat_s.size()-1];
    std::cout << "data left merge with default !"<<std::endl;
    if(data_right != data_left)
    {
        bool row_node_tmp = false;
        row_group_merge(Bdd_tree,node_info,data_left,data_right,delay_left,delay_right,row_node_tmp);
        if(node_info.row_MUXgroup.size() >0 && row_node_tmp == false)
        {    bool flag_tmp = false;
            for(int z = 0; z<node_info.row_MUXgroup.size(); z++)
            {
                if(node_info.row_MUXgroup[z].get_low() == data_left && node_info.row_MUXgroup[z].get_high() == data_right)
                {
                    std::cout <<"The current node already exists !"<<std::endl;
                    std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                    node_info.data_s.push_back(node_info.row_MUXgroup[z].get_out()); 
                    node_info.addr_i.push_back(addr_h);
                    flag_tmp = true;

                    //Delay info
                    // auto max_delay = BDD::max_data(delay_left,delay_right);
                    auto max_delay = node_info.delay_tmp[z];
                    node_info.delay_tmp.push_back(max_delay);
                }
            }
            if(flag_tmp == false)
            {
                node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                std::cout<<"new MUX nod2 is being created !"<<std::endl;
                std::string node_type = "MUX Node"; 
                //create MUX node
                BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                node_info.row_MUXgroup.push_back(node_MUX);
                Bdd_tree.Tree_level_c ++;
                node_info.data_s.push_back(node_info.var_out); 
                node_info.addr_i.push_back(addr_h);

                //Delay info
                auto max_delay = BDD::max_data(delay_left,delay_right);
                max_delay = max_delay + MUX_Delay;
                node_info.delay_tmp.push_back(max_delay);
            }
        }else if(row_node_tmp == false)
        {       
            node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
            std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
            std::cout<<"first MUX node2 is being created !"<<std::endl;
            std::string node_type = "MUX Node"; 
            //create MUX node
            BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
            node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
            node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
            node_info.row_MUXgroup.push_back(node_MUX);
            Bdd_tree.Tree_level_c ++;
            node_info.data_s.push_back(node_info.var_out); 
            node_info.addr_i.push_back(addr_h);

            //Delay info
            auto max_delay = BDD::max_data(delay_left,delay_right);
            max_delay = max_delay + MUX_Delay;
            node_info.delay_tmp.push_back(max_delay);
        }
    }else
    {
        node_info.var_out = Bdd_tree.datamat_s[i];
        std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
        node_info.data_s.push_back(node_info.var_out);
        node_info.addr_i.push_back(addr_h);

        //Delay info
        auto max_delay = BDD::max_data(delay_left,delay_right);
        node_info.delay_tmp.push_back(max_delay);
    }
}

void BDD::Mux_node2(Tree& Bdd_tree,Node_info& node_info,int& i)
{
    auto data_left = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-1];
    auto delay_left = Bdd_tree.delay[Bdd_tree.datamat_s.size()-1];
    auto data_right = Bdd_tree.datamat_s[i];
    auto delay_right = Bdd_tree.delay[i];

    std::cout << "data right merge with default !"<<std::endl;
    if(data_left != data_right)
    {
        bool row_node_tmp = false;
        row_group_merge(Bdd_tree,node_info,data_left,data_right,delay_left,delay_right,row_node_tmp);
        if(node_info.row_MUXgroup.size() >0 && row_node_tmp == false)
        {    bool flag_tmp = false;
            for(int z = 0; z<node_info.row_MUXgroup.size(); z++)
            {
                if(node_info.row_MUXgroup[z].get_low() == data_left && node_info.row_MUXgroup[z].get_high() == data_right)
                {
                    std::cout <<"The current node already exists !"<<std::endl;
                    std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                    node_info.data_s.push_back(node_info.row_MUXgroup[z].get_out()); 
                    node_info.addr_i.push_back(addr_h);
                    flag_tmp = true;

                    //Delay info
                    // auto max_delay = BDD::max_data(delay_left,delay_right);
                    auto max_delay = node_info.delay_tmp[z];
                    node_info.delay_tmp.push_back(max_delay);
                }
            }
            if(flag_tmp == false)
            {
                node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                std::cout<<"new MUX node3 is being created !"<<std::endl;
                std::string node_type = "MUX Node"; 
                //create MUX node
                BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                node_info.row_MUXgroup.push_back(node_MUX);
                Bdd_tree.Tree_level_c ++;
                node_info.data_s.push_back(node_info.var_out); 
                node_info.addr_i.push_back(addr_h);

                //Delay info
                auto max_delay = BDD::max_data(delay_left,delay_right);
                max_delay = max_delay + MUX_Delay;
                node_info.delay_tmp.push_back(max_delay);
            }
        }else if(row_node_tmp == false){
                node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                std::cout<<"first MUX node3 is being created !"<<std::endl;
                std::string node_type = "MUX Node"; 
                //create MUX node
                BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                node_info.row_MUXgroup.push_back(node_MUX);
                Bdd_tree.Tree_level_c ++;
                node_info.data_s.push_back(node_info.var_out); 
                node_info.addr_i.push_back(addr_h);
                           
                //Delay info
                auto max_delay = BDD::max_data(delay_left,delay_right);
                max_delay = max_delay + MUX_Delay;
                node_info.delay_tmp.push_back(max_delay);
        }
    }else
    {
        node_info.var_out = Bdd_tree.datamat_s[i];
        std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
        node_info.data_s.push_back(node_info.var_out);
        node_info.addr_i.push_back(addr_h);

        //Delay info
        auto max_delay = BDD::max_data(delay_left,delay_right);
        node_info.delay_tmp.push_back(max_delay);
    }
}

void BDD::Mux_node3(Tree& Bdd_tree,Node_info& node_info,int& i)
{
    std::cout<<"The last data is being selected.............."<<std::endl;
    auto last_addr =  Bdd_tree.addrmat[Bdd_tree.addrmat.size()-1];
    auto last_addr_l = last_addr.back();
    if (last_addr_l == 0  )
    {
        auto data_left = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-2];
        auto delay_left = Bdd_tree.delay[Bdd_tree.datamat_s.size()-2];
        auto data_right = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-1];
        auto delay_right = Bdd_tree.delay[Bdd_tree.datamat_s.size()-1];

        std::cout << "the last data left merge with default !"<<std::endl;
        if(data_right != data_left)
        {
            bool row_node_tmp = false;
            row_group_merge(Bdd_tree,node_info,data_left,data_right,delay_left,delay_right,row_node_tmp);
            
           if(node_info.row_MUXgroup.size() >0 && row_node_tmp == false)
            {   bool flag_tmp = false;
                for(int z = 0; z<node_info.row_MUXgroup.size(); z++)
                {
                    if(node_info.row_MUXgroup[z].get_low() == data_left && node_info.row_MUXgroup[z].get_high() == data_right)
                    {
                        std::cout <<"The current node already exists !"<<std::endl;
                        std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                        node_info.data_s.push_back(node_info.row_MUXgroup[z].get_out()); 
                        node_info.addr_i.push_back(addr_h);
                        flag_tmp = true;

                        //Delay info
                        // auto max_delay = BDD::max_data(delay_left,delay_right);
                        auto max_delay = node_info.delay_tmp[z];
                        node_info.delay_tmp.push_back(max_delay);
                    }
                }
                if(flag_tmp == false)
                {
                    node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                    std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                    std::cout<<"new MUX node4 is being created !"<<std::endl;
                    std::string node_type = "MUX Node"; 
                    //create MUX node
                    BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                    node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                    node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                    node_info.row_MUXgroup.push_back(node_MUX);
                    Bdd_tree.Tree_level_c ++;
                    node_info.data_s.push_back(node_info.var_out); 
                    node_info.addr_i.push_back(addr_h);

                    //Delay info
                    auto max_delay = BDD::max_data(delay_left,delay_right);
                    max_delay = max_delay + MUX_Delay;
                    node_info.delay_tmp.push_back(max_delay);
                }
            }else if(row_node_tmp == false){
                    node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                    std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                    std::cout<<"first MUX node4 is being created !"<<std::endl;
                    std::string node_type = "MUX Node"; 
                    //create MUX node
                    BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                    node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                    node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                    node_info.row_MUXgroup.push_back(node_MUX);
                    Bdd_tree.Tree_level_c ++;
                    node_info.data_s.push_back(node_info.var_out); 
                    node_info.addr_i.push_back(addr_h);

                    //Delay info
                    auto max_delay = BDD::max_data(delay_left,delay_right);
                    max_delay = max_delay + MUX_Delay;
                    node_info.delay_tmp.push_back(max_delay);
            }
        }else{
            node_info.var_out = Bdd_tree.datamat_s[i];
            std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
            node_info.data_s.push_back(node_info.var_out);
            node_info.addr_i.push_back(addr_h);

            //Delay info
            auto max_delay = BDD::max_data(delay_left,delay_right);
            node_info.delay_tmp.push_back(max_delay);
        }
    }else if(last_addr_l == 1)
    {
        auto data_left = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-1];
        auto delay_left = Bdd_tree.delay[Bdd_tree.datamat_s.size()-1];
        auto data_right = Bdd_tree.datamat_s[Bdd_tree.datamat_s.size()-2];
        auto delay_right = Bdd_tree.delay[Bdd_tree.datamat_s.size()-2];

        std::cout << "the last data right merge with default !"<<std::endl;
        if(data_left != data_right)
        {
            bool row_node_tmp = false;
            row_group_merge(Bdd_tree,node_info,data_left,data_right,delay_left,delay_right,row_node_tmp);
           
            if(node_info.row_MUXgroup.size() >0 && row_node_tmp == false)
            {   bool flag_tmp = false;
                for(int z = 0; z<node_info.row_MUXgroup.size(); z++)
                {
                    if(node_info.row_MUXgroup[z].get_low() == data_left && node_info.row_MUXgroup[z].get_high() == data_right)
                    {
                        std::cout <<"The current node already exists !"<<std::endl;
                        std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                        node_info.data_s.push_back(node_info.row_MUXgroup[z].get_out()); 
                        node_info.addr_i.push_back(addr_h);
                        flag_tmp = true;

                        //Delay info
                        // auto max_delay = BDD::max_data(delay_left,delay_right);
                        auto max_delay = node_info.delay_tmp[z];
                        node_info.delay_tmp.push_back(max_delay);
                    }
                }
                if(flag_tmp == false)
                {
                    node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                    std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                    std::cout<<"new MUX node5 is being created !"<<std::endl;
                    std::string node_type = "MUX Node"; 
                    //create MUX node
                    BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                    node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                    node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                    node_info.row_MUXgroup.push_back(node_MUX);
                    Bdd_tree.Tree_level_c ++;
                    node_info.data_s.push_back(node_info.var_out); 
                    node_info.addr_i.push_back(addr_h);

                    //Delay info
                    auto max_delay = BDD::max_data(delay_left,delay_right);
                    max_delay = max_delay + MUX_Delay;  
                    node_info.delay_tmp.push_back(max_delay);
                }
            }else if(row_node_tmp == false){
                    node_info.var_out = "n"+ std::to_string(Bdd_tree.tree_num)+ std::to_string(Bdd_tree.Tree_level_r) + std::to_string(Bdd_tree.Tree_level_c);
                    std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
                    std::cout<<"first MUX node5 is being created !"<<std::endl;
                    std::string node_type = "MUX Node"; 
                    //create MUX node
                    BDD::Node node_MUX(data_left,data_right,node_info.sel_s,node_info.var_out,node_type);
                    node_MUX.Node_level_c = Bdd_tree.Tree_level_c;
                    node_MUX.Node_level_r = Bdd_tree.Tree_level_r;
                    node_info.row_MUXgroup.push_back(node_MUX);
                    Bdd_tree.Tree_level_c ++;
                    node_info.data_s.push_back(node_info.var_out); 
                    node_info.addr_i.push_back(addr_h);

                    //Delay info
                    auto max_delay = BDD::max_data(delay_left,delay_right);
                    max_delay = max_delay + MUX_Delay;
                    node_info.delay_tmp.push_back(max_delay);
            }
        }else
        {
            node_info.var_out = Bdd_tree.datamat_s[i];
            std::vector<int> addr_h(last_addr.begin(),last_addr.end()-1);
            node_info.data_s.push_back(node_info.var_out);
            node_info.addr_i.push_back(addr_h);

            //Delay info
            auto max_delay = BDD::max_data(delay_left,delay_right);
            node_info.delay_tmp.push_back(max_delay);
        }
    }
}

float BDD::max_data(float& a,float& b)
{
    if( a > b)
    {
        return a;
    }else{
        return b;
    }
}

void BDD::row_group_merge(Tree& Bdd_tree,Node_info& node_info,std::string& data_left,std::string& data_right,float& delay_left,float& delay_right,bool& row_node_tmp)
{
    //Determine whether the node of the current groups 
    //can be merged with the previous group
    for(int t = Bdd_tree.groups.size()-1; t >= 0; t--)
    {    
        auto &pre_group = Bdd_tree.groups[t];

        //Check whether nodes of two groups can be fused
        for(int node_n = 0; node_n<pre_group.size();node_n++)
        {
            if(pre_group[node_n].out == data_left && pre_group[node_n].low == data_right)
            {
                std::cout<<"pre group "<< t <<"and current group can be merged 01"<<std::endl;
                bool sel_not_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(node_info.sel_s == tmp_sel_node.low && tmp_sel_node.Node_type == "NOT Node")
                    {
                        std::cout<<"already have same NOT node : "<<std::endl;
                        std::string sel_not_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_not_var);
                        sel_not_flag = true;
                        break;
                    }
                }
                if(sel_not_flag == false)
                {
                    std::string NOT_type = "NOT Node";
                    std::string sel_not_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);
                    node_info.sel_var.push_back(sel_not_var);
                    std::cout<<"1sel not var : "<<sel_not_var<<std::endl;

                    BDD::Node node_NOT(node_info.sel_s,node_info.sel_var.back(),NOT_type); 
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_NOT);
                }

                bool sel_and_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(pre_group[node_n].sel == tmp_sel_node.low && node_info.sel_var.back() == tmp_sel_node.high && tmp_sel_node.Node_type =="AND Node")
                    {
                        std::cout<<"already have same AND node : "<<std::endl;
                        std::string sel_and_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_and_var);
                        pre_group[node_n].sel = node_info.sel_var.back();
                        sel_and_flag = true;
                        break;
                    }
                }
                if(sel_and_flag == false)
                {
                    std::string AND_type = "AND Node";
                    std::string sel_and_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);           
                    std::cout<<"1sel and var : "<<sel_and_var<<std::endl;
                    BDD::Node node_AND(pre_group[node_n].sel,node_info.sel_var.back(),sel_and_var,AND_type);
                    node_info.sel_var.push_back(sel_and_var);
                    pre_group[node_n].sel = node_info.sel_var.back();
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_AND);
                }
                // node_info.var_out = Bdd_tree.datamat_s[i];
                // std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                // node_info.data_s.push_back(node_info.var_out);
                // node_info.addr_i.push_back(addr_h);
                row_node_tmp = true;
        
                node_info.var_out = pre_group[node_n].out;
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                node_info.data_s.push_back(node_info.var_out);
                node_info.addr_i.push_back(addr_h);
                //Delay info
                auto max_delay1 = BDD::max_data(delay_left,delay_right);
                if(sel_not_flag == false && sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + NOT_Delay + AND_Delay;
                }else if(sel_not_flag == true && sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + AND_Delay;
                }else if(sel_not_flag == false && sel_and_flag == true)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + NOT_Delay ;
                }else 
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay ;

                }
                auto max_delay = BDD::max_data(max_delay1,pre_group[node_n].sel_delay);
                node_info.delay_tmp.push_back(max_delay);
                break;
            }else if(pre_group[node_n].out == data_left && pre_group[node_n].high == data_right)
            {
                std::cout<<"pre group and group can be merged 02"<<std::endl;
                bool sel_not_flag1 = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(node_info.sel_s == tmp_sel_node.low && tmp_sel_node.Node_type == "NOT Node")
                    {
                        std::cout<<"already have same NOT node : "<<std::endl;
                        std::string sel_not_var1 = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_not_var1);
                        sel_not_flag1 = true;
                        break;
                    }
                }
                if(sel_not_flag1 == false)
                {
                std::string NOT_type1 = "NOT Node";
                std::string sel_not_var1 = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);
                std::cout<<"2sel not var1 : "<<sel_not_var1<<std::endl;
                node_info.sel_var.push_back(sel_not_var1);
                BDD::Node node_NOT1(node_info.sel_s,node_info.sel_var.back(),NOT_type1); 
                Bdd_tree.Sel_level_c ++;
                Bdd_tree.sel_groups.push_back(node_NOT1);
                }

                bool sel_not_flag2 = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(pre_group[node_n].sel == tmp_sel_node.low && tmp_sel_node.Node_type == "NOT Node")
                    {
                        std::cout<<"already have same NOT node : "<<std::endl;
                        std::string sel_not_var2 = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_not_var2);
                        sel_not_flag2 = true;
                        break;
                    }
                }
                if(sel_not_flag2 == false)
                {
                    std::string NOT_type2 = "NOT Node";
                    std::string sel_not_var2 = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);
                    std::cout<<"2sel not var2 : "<<sel_not_var2<<std::endl;
                    node_info.sel_var.push_back(sel_not_var2);
                    BDD::Node node_NOT2(pre_group[node_n].sel,node_info.sel_var.back(),NOT_type2); 
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_NOT2);
                }

                bool sel_and_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(node_info.sel_var[node_info.sel_var.size()-2] == tmp_sel_node.low && node_info.sel_var[node_info.sel_var.size()-1] == tmp_sel_node.high && tmp_sel_node.Node_type =="AND Node")
                    {
                        std::cout<<"already have same AND node : "<<std::endl;
                        std::string sel_and_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_and_var);
                        pre_group[node_n].sel = node_info.sel_var.back();
                        sel_and_flag = true;
                        break;
                    }
                }
                if(sel_and_flag == false)
                {
                    std::string AND_type = "AND Node";
                    std::string sel_and_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);           
                    std::cout<<"2sel and var  : "<<sel_and_var<<std::endl;
                    BDD::Node node_AND(node_info.sel_var[node_info.sel_var.size()-2],node_info.sel_var[node_info.sel_var.size()-1],sel_and_var,AND_type);
                    node_info.sel_var.push_back(sel_and_var);
                    pre_group[node_n].sel = node_info.sel_var.back();
                    auto pre_low = pre_group[node_n].low;
                    pre_group[node_n].low = pre_group[node_n].high;
                    pre_group[node_n].high = pre_low;
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_AND);
                }
                // node_info.var_out = Bdd_tree.datamat_s[i];
                // std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                // node_info.data_s.push_back(node_info.var_out);
                // node_info.addr_i.push_back(addr_h);
                row_node_tmp = true;
        
                node_info.var_out = pre_group[node_n].out;
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                node_info.data_s.push_back(node_info.var_out);
                node_info.addr_i.push_back(addr_h);
                //Delay info
                auto max_delay1 = BDD::max_data(delay_left,delay_right);
                if(sel_not_flag1 == true &&sel_not_flag2 == true && sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + AND_Delay;
                }else if(sel_not_flag1 == true && sel_not_flag2 == true &&sel_and_flag == true)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay ;
                }else if((sel_not_flag1 == true && sel_not_flag2 == false) || (sel_not_flag1 == false && sel_not_flag2 == true))
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + NOT_Delay ;
                }else 
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + AND_Delay + NOT_Delay ;
                }
                auto max_delay = BDD::max_data(max_delay1,pre_group[node_n].sel_delay);
                node_info.delay_tmp.push_back(max_delay);
                break;
            }else if(pre_group[node_n].out == data_right && pre_group[node_n].low == data_left)
            {
                std::cout<<"pre group and group can be merged 03"<<std::endl;
                // std::string NOT_type = "NOT Node";
                // std::string sel_not_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);
                // node_info.sel_var.push_back(sel_not_var);
                // BDD::Node node_NOT(node_info.sel_s,node_info.sel_var.back(),NOT_type); 
                // Bdd_tree.Sel_level_c ++;
                // Bdd_tree.sel_groups.push_back(node_NOT);
                bool sel_and_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(pre_group[node_n].sel == tmp_sel_node.low && node_info.sel_s == tmp_sel_node.high && tmp_sel_node.Node_type =="AND Node")
                    {
                        std::cout<<"already have same AND node : "<<std::endl;
                        std::string sel_and_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_and_var);
                        pre_group[node_n].sel = node_info.sel_var.back();
                        sel_and_flag = true;
                        break;
                    }
                }
                if(sel_and_flag == false)
                {
                    std::string AND_type = "AND Node";
                    std::string sel_and_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);           
                    std::cout<<"3sel and var : "<<sel_and_var<<std::endl;
                    BDD::Node node_AND(pre_group[node_n].sel,node_info.sel_s,sel_and_var,AND_type);
                    node_info.sel_var.push_back(sel_and_var);
                    pre_group[node_n].sel = node_info.sel_var.back();
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_AND);
                }
                // node_info.var_out = Bdd_tree.datamat_s[i];
                // std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                // node_info.data_s.push_back(node_info.var_out);
                // node_info.addr_i.push_back(addr_h);
                row_node_tmp = true;
                node_info.var_out = pre_group[node_n].out;
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                node_info.data_s.push_back(node_info.var_out);
                node_info.addr_i.push_back(addr_h);
        
                //Delay info
                auto max_delay1 = BDD::max_data(delay_left,delay_right);
                if(sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay +  AND_Delay;
                }else 
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay ;
                }
                auto max_delay = BDD::max_data(max_delay1,pre_group[node_n].sel_delay);
                node_info.delay_tmp.push_back(max_delay);
                break;
            }else if(pre_group[node_n].out == data_right && pre_group[node_n].high == data_left)
            {
                std::cout<<"pre group and group can be merged 04"<<std::endl;
                bool sel_not_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(pre_group[node_n].sel== tmp_sel_node.low && tmp_sel_node.Node_type == "NOT Node")
                    {
                        std::cout<<"already have same NOT node : "<<std::endl;
                        std::string sel_not_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_not_var);
                        sel_not_flag = true;
                        break;
                    }
                }
                if(sel_not_flag == false)
                {
                    std::string NOT_type = "NOT Node";
                    std::string sel_not_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);
                    std::cout<<"4sel not var : "<<sel_not_var<<std::endl;
                    node_info.sel_var.push_back(sel_not_var);
                    BDD::Node node_NOT(pre_group[node_n].sel,node_info.sel_var.back(),NOT_type); 
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_NOT);
                }
                bool sel_and_flag = false;
                for(int x = 0 ; x < Bdd_tree.sel_groups.size() ; x++)
                {
                    auto tmp_sel_node = Bdd_tree.sel_groups[x];
                    if(node_info.sel_s == tmp_sel_node.low && node_info.sel_var.back() == tmp_sel_node.high && tmp_sel_node.Node_type =="AND Node")
                    {
                        std::cout<<"already have same AND node : "<<std::endl;
                        std::string sel_and_var = tmp_sel_node.out;
                        node_info.sel_var.push_back(sel_and_var);
                        sel_and_flag = true;
                        break;
                    }
                }
                if(sel_and_flag == false)
                { 
                    std::string AND_type = "AND Node";
                    std::string sel_and_var = "s"+std::to_string(Bdd_tree.tree_num) + std::to_string(Bdd_tree.Sel_level_r) + std::to_string(Bdd_tree.Sel_level_c);           
                    std::cout<<"4sel and var : "<<sel_and_var<<std::endl;
                    BDD::Node node_AND(node_info.sel_s,node_info.sel_var.back(),sel_and_var,AND_type);
                    node_info.sel_var.push_back(sel_and_var);
                    pre_group[node_n].sel = node_info.sel_var.back();
                    
                    Bdd_tree.Sel_level_c ++;
                    Bdd_tree.sel_groups.push_back(node_AND);
                }
                // node_info.var_out = Bdd_tree.datamat_s[i];
                // std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                // node_info.data_s.push_back(node_info.var_out);
                // node_info.addr_i.push_back(addr_h);
                row_node_tmp = true;
                node_info.var_out = pre_group[node_n].out;
                std::vector<int> addr_h(node_info.addr_left.begin(),node_info.addr_left.end()-1);
                node_info.data_s.push_back(node_info.var_out);
                node_info.addr_i.push_back(addr_h);
        
                //Delay info
                auto max_delay1 = BDD::max_data(delay_left,delay_right);
                if(sel_not_flag == false && sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + NOT_Delay + AND_Delay;
                }else if(sel_not_flag == false && sel_and_flag == true)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + NOT_Delay;
                }else if(sel_not_flag == true && sel_and_flag == false)
                {
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay + AND_Delay;
                }else{
                    pre_group[node_n].sel_delay = pre_group[node_n].sel_delay;
                }

                auto max_delay = BDD::max_data(max_delay1,pre_group[node_n].sel_delay);
                node_info.delay_tmp.push_back(max_delay);
                break;
            }
        }
        if(row_node_tmp == true)
        {
            break;
        }
    }
}
/**
 * Visualize the structure of the tree
 * */
 void BDD::visual_tree(Tree& Bdd_tree)
{
    for(int i = 0; i<Bdd_tree.groups.size(); i++)
    {
        for(int j = 0; j<Bdd_tree.groups[i].size(); j++)
        {
            auto& tmp = Bdd_tree.groups[i][j];
            std::cout<<"Visual node ................"<<std::endl;
            // std::cout<<"Bdd_tree.groups.size()."<<Bdd_tree.groups.size()<<std::endl;
            std::cout<<"Bdd_tree row : "<<i<<" , ";
            std::cout<<"Bdd_tree col : "<<j<<std::endl;

            if(tmp.Node_type == "NOT Node")
            {
                std::cout<<"node type: "<<tmp.Node_type<<std::endl;
                std::cout<<"node low input: "<<tmp.low<<std::endl;
                std::cout<<"node out: "<<tmp.out<<std::endl;

            }else if(tmp.Node_type == "MUX Node")
            {
                std::cout<<"node type: "<<tmp.Node_type<<std::endl;
                std::cout<<"node low input: "<<tmp.low<<std::endl;
                std::cout<<"node high input: "<<tmp.high<<std::endl;
                std::cout<<"node sel: "<<tmp.sel<<std::endl;
                std::cout<<"node out: "<<tmp.out<<std::endl;

            }
        }
    }
    for(int i = 0; i<Bdd_tree.sel_groups.size(); i++)
    {
        auto tmp = Bdd_tree.sel_groups[i];
        if(tmp.Node_type == "NOT Node")
        {
            std::cout<<"node type: "<<tmp.Node_type<<std::endl;
            std::cout<<"node low input: "<<tmp.low<<std::endl;
            std::cout<<"node out: "<<tmp.out<<std::endl;

        }else if(tmp.Node_type == "AND Node")
        {
            std::cout<<"node type: "<<tmp.Node_type<<std::endl;
            std::cout<<"node low input: "<<tmp.low<<std::endl;
            std::cout<<"node high input: "<<tmp.high<<std::endl;
            std::cout<<"node out: "<<tmp.out<<std::endl;
        }
    }
}