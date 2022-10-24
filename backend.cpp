#include "backend.h"
#include <fstream>
#include "BDD.h"
using namespace std;
using namespace BDD;

float evaluation(Tree& tree)
{
    //Evaluate the area of BDD
    float area = 0;
    for(int i = 0; i<tree.groups.size(); i++)
    {
        for(int j = 0; j<tree.groups[i].size(); j++)
        {
            auto& tmp = tree.groups[i][j];
            if(tmp.Node_type == "MUX Node")
            {
                area += 2;
            }
            else if(tmp.Node_type == "NOT Node")
            {
                area += 0.1;
            }else if(tmp.Node_type == "AND Node")
            {
                area += 1;
            }
        }
    }
    for(int k = 0; k<tree.sel_groups.size(); k++)
    {
        auto& sel_tmp = tree.sel_groups[k];
        if(sel_tmp.Node_type == "NOT Node")
        {
            area += 0.1;
        }else if(sel_tmp.Node_type == "AND Node")
        {
            area += 1;
        }
    }
    // cout << "net area = " << area << endl;
    // cout << "net delay= " << tree.delay[0] << endl;
    return area;
}
void outputveri(int *inout_info, Tree &tree, int bit_num)
{
    // open a file for outputting VERILOG
    ofstream outputfile;
    outputfile.open ("out.v", ios::app);
    //output verilog
    if (outputfile.is_open()) {
        //output module inout_info
        if(bit_num == 0)
        {
            outputfile << "module mux_out(" << endl;
            outputfile << "\t" << "input [" << inout_info[0] << ":0] sel," << endl;
            outputfile << "\t" << "input [" << inout_info[1] << ":0] din," << endl;
            outputfile << "\t" << "output [" << inout_info[2] << ":0] dout" << endl;
            outputfile << ");" << endl << endl;
        }
        //output wire
        outputfile <<"wire ";
        int flag = 0;
        for(int i = 0; i<tree.groups.size(); i++)
        {
            for(int j = 0; j<tree.groups[i].size(); j++)
            {
                auto& tmp = tree.groups[i][j];
                if(flag == 0){
                    outputfile <<tmp.out;
                    flag = 1;
                } else {
                    outputfile <<", "<<tmp.out;
                }
            }
        }
        outputfile << ";" << endl << endl;

        //output assign
        for(int i = 0; i < tree.groups.size(); i++)
        {
            for(int j = 0; j < tree.groups[i].size(); j++)
            {
                auto& tmp = tree.groups[i][j];
                if(tmp.Node_type == "MUX Node")
                {
                    outputfile << "assign " << tmp.out << " = " 
                    << "(" << tmp.low << " & " << "~" << tmp.sel << ")" 
                    << " | "
                    << "(" << tmp.high << " & " << tmp.sel << ")" 
                    << ";" << endl;
                }
                else if(tmp.Node_type == "NOT Node")
                {
                    outputfile << "assign " << tmp.out << " = " 
                    << "~" << tmp.low
                    << ";" << endl;
                }else if(tmp.Node_type == "AND Node")
                {
                    outputfile << "assign " << tmp.out << " = " 
                    << tmp.low << " & " << tmp.high 
                    << ";" << endl;
                }

                if(i == (tree.groups.size() - 1) && j == (tree.groups[i].size() - 1))
                {
                    outputfile <<"assign " << "dout[" << bit_num << "] = " << tmp.out << ";" << endl << endl ;
                }
            }
        }

        //output sel_assign
        for(int k = 0; k<tree.sel_groups.size(); k++)
        {
            auto& sel_tmp = tree.sel_groups[k];
            if(sel_tmp.Node_type == "NOT Node")
            {
                outputfile << "assign " << sel_tmp.out << " = " 
                << "~" << sel_tmp.low
                << ";" << endl;
            }else if(sel_tmp.Node_type == "AND Node")
            {
                outputfile << "assign " << sel_tmp.out << " = " 
                << sel_tmp.low << " & " << sel_tmp.high 
                << ";" << endl;
            }
        }
        //output end module
        if(bit_num == inout_info[2])
        {
            outputfile << endl << "endmodule";
        }
    }else{
        cout<<"open outputfile failed!"<<endl;
    }

    outputfile.close();

}