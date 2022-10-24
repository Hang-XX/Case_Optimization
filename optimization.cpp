#include <iostream>
#include <vector>
#include <assert.h>
#include <map>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "common.h"

#include "dataparsing.h"
#include "Quine.h"
#include "BDD.h"
#include "backend.h"

using namespace std;
using namespace BDD;

void optimize_case(MATRIX addr_mat,MATRIX data_mat,RANGE din_range,RANGE dout_range,RANGE sel_range)
{	
	// std::vector<std::vector<int>> addrMatrix = {
	// 	{1, 0, 1, 0},
	// 	{1, -1, -1, 1},
	// 	{0, 0, 1, 0 },
	// 	{0, 1, -1, 0}
	// };
    
	// std::vector<std::vector<int>> dataMatrix = {
	// 	{12, 11, 0},
	// 	{1, 4, 8},
	// 	{10, 1, 0},
	// 	{10, 9, 6},
	// 	{6, 4, 2}
	// };

    std::vector<std::vector<int>> addrMatrix  = addr_mat;
    std::vector<std::vector<int>> dataMatrix  = data_mat;
    // std::vector<std::vector<int>> dataMatrix = {
	// 	{12},
	// 	{1},
	// 	{10},
	// 	{10},
	// 	{6}
	// };

    int addrMat_row_num = addrMatrix.size();
	int addrMat_col_num = addrMatrix[0].size();
	int dataMat_row_num = dataMatrix.size();
	int dataMat_col_num = dataMatrix[0].size();
    std::cout << "addrMatrix row number: " << addrMat_row_num << std::endl;
	std::cout << "addrMatrix col number: " << addrMat_col_num << std::endl;
	std::cout << "dataMatrix row number: " << dataMat_row_num << std::endl;
	std::cout << "dataMatrix col number: " << dataMat_col_num << std::endl;
    
	assert(addrMat_row_num == dataMat_row_num - 1);
    PreprocessData p;
    float area_all = 0;
    float area_bit;
    float delay_bit;
    float delay_max = 0;
    for (int i = 0; i < dataMat_col_num; i++) {
        std::vector<std::vector<int>> addrMatrix_output;
        std::vector<int> dataMatrix_onecol_output;
        std::vector<std::vector<int>> sel_list_vec;
        std::vector<int> data_onecol_vec;

		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "Process datamatrix column: " << i << std::endl;
        
        //--------------------------------------------------------------------
        //1、pre-process address matrix and data matrix
        //--------------------------------------------------------------------
        for (int j = 0; j < addrMat_row_num; j++) {
            stringstream sel_row_ss;
            string sel_row_str;
            copy(addrMatrix[j].begin(), addrMatrix[j].end(), ostream_iterator<int>(sel_row_ss, ""));
            sel_row_str = sel_row_ss.str();
            int negetive1_num = 0;
            negetive1_num = count(addrMatrix[j].begin(), addrMatrix[j].end(), -1);
            std::cout << sel_row_str << "\t" << "-1(?) exists and the number of -1(?) is: " << negetive1_num << std::endl;

            //if -1(?) exists in the row line of address matrix?
            if (negetive1_num == 0) {
                sel_list_vec.push_back(addrMatrix[j]);
                data_onecol_vec.push_back(dataMatrix[j][i]);
            } 
            else {
                //recorde the start index of row line
                int start_row_index = sel_list_vec.size();
                std::cout << "start row index: " << start_row_index << std::endl;
                //push_back negetive1_num times addrMat[j] in vector::sel_list_vec;
                //sel_list_vec.reserve(pow(2, addrMat_col_num));//keep vector capacity enough
                for (int k = 0; k < pow(2, negetive1_num); k++) {
                    sel_list_vec.push_back(addrMatrix[j]);
                    data_onecol_vec.push_back(dataMatrix[j][i]);

                }

                //find -1(?) index in this row line
                int searching_element = -1;
                std::vector<int> vec_of_indices;
                for (size_t index = 0; index < 4; index++) {
                    if (sel_list_vec[start_row_index].at(index) == searching_element) {
                        vec_of_indices.push_back(index);
                    }
                }

                for (int t = 0; t < vec_of_indices.size(); t++) {
                    std::cout << "-1 index in this row line: " << vec_of_indices[t] << std::endl;
                }
                
                //replace -1(?) with 1 and 0
                int cnt = 0;
                for (int m = start_row_index; m <= (start_row_index + pow(2, negetive1_num)) - 1; m++) {
                    std::string binform = p.decToBin(cnt, negetive1_num);
                    const char *pc = binform.c_str();
                    for (int t = 0; t < vec_of_indices.size(); t++) {
                        std::stringstream binform_ss;
                        int binform_int;
                        binform_ss << binform[t];
                        binform_ss >> binform_int;
                        sel_list_vec[m][vec_of_indices[t]] = binform_int;
                    }
                    cnt = cnt + 1;
                }
                vec_of_indices.clear();
            }
        }
        
        std::vector<std::vector<int> >::iterator p1;
        std::vector<int>::iterator p2;
        std::cout << "complete sel_list_vec shows as below:" << std::endl;
        for (p1 = sel_list_vec.begin(); p1 != sel_list_vec.end(); p1++) {
            for (p2 = p1->begin(); p2 != p1->end(); p2++) {
                std::cout << *p2 << "\t";
            }
            std::cout << std::endl;
        }
        std::vector<int>::iterator p3;
        std::cout << "data_onecol_vec shows as below:" << std::endl;
        for (p3 = data_onecol_vec.begin(); p3 != data_onecol_vec.end(); p3++) {
            std::cout << *p3 << std::endl;
        } 
        p.BoolOutput(sel_list_vec, data_onecol_vec, addrMat_col_num, addrMatrix_output, dataMatrix_onecol_output);
        std::cout << "--------------------------------------------------------" << std::endl;
        std::cout << "Precoss data done!" << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;

        //--------------------------------------------------------------------
        //2、BDD tree
        //--------------------------------------------------------------------
        std::cout << "default: " << dataMatrix[dataMat_row_num-1][i] << std::endl;
        dataMatrix_onecol_output.push_back(dataMatrix[dataMat_row_num-1][i]);
        BDD::Tree Bdd_tree(0,0,0,0, addrMatrix_output, dataMat_col_num);
        for(int i = 0; i<addrMatrix_output.size(); i++)
        {
            for (int j = 0; j<addrMatrix_output[i].size(); j++)
            {
                cout<<"addrMatrix output : "<<i<<","<<j<<" : "<<addrMatrix_output[i][j]<<endl;
            }
        }
        for(int i = 0; i<dataMatrix_onecol_output.size(); i++)
        {
            cout<<"dataMatrix one col output :"<<dataMatrix_onecol_output[i]<<endl;
        }
        create_bdd(addrMatrix_output, dataMatrix_onecol_output, Bdd_tree);
        visual_tree(Bdd_tree);

        //--------------------------------------------------------------------
        //3、evaluate area and delay 
        //--------------------------------------------------------------------
        area_bit = evaluation(Bdd_tree);
        area_all += area_bit;
        cout << "area of dout[" << dataMat_col_num << "] = " << area_bit << endl;
        if( i == dataMat_col_num - 1)
        { 
            cout << "****************************" << endl;
            cout << "        area_all =   " << area_all << endl;
            cout << "****************************" << endl << endl;
        }

        delay_bit = Bdd_tree.delay[0];
        cout << "delay of dout[" << dataMat_col_num << "] = " << delay_bit << endl;
        delay_max = delay_bit > delay_max ? delay_bit : delay_max;
        if( i == dataMat_col_num - 1)
        { 
            cout << "****************************" << endl;
            cout << "        delay_max =   " << delay_max << endl;
            cout << "****************************" << endl;
        }
        //--------------------------------------------------------------------
        //4、map BDD tree to circuit and output out.v
        //--------------------------------------------------------------------
        int inout_info[3] = {sel_range.at(0), din_range.at(0), dout_range.at(0)};
        outputveri(inout_info, Bdd_tree, i);

        //--------------------------------------------------------------------
        //5、clear vector
        //--------------------------------------------------------------------
        addrMatrix_output.clear();
        dataMatrix_onecol_output.clear();
        sel_list_vec.clear();
        data_onecol_vec.clear();
        
    }
    
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "The whole process has done successfully!" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
    
}
