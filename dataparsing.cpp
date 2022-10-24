#include <iostream>
#include <vector>
#include <assert.h>
#include <map>
#include "dataparsing.h"
#include <iterator>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstring>
#include <iomanip>
using namespace std;

std::string PreprocessData::decToBin(int n, int binWidth) {
	int result = 0;
	int k = 1;
	int i = 0;
	while (n) {
		i = n % 2;
		result = k * i + result;
		k = k * 10;
		n = n / 2;
	}
	std::stringstream ss;
	std::string str;
	std::string zero =  "0";
	ss << result;
	ss >> str;
	while (str.length() < binWidth) {
		str = zero + str ;
	}
	return str;
}

int PreprocessData::BinTodec(int bin)
{
	stringstream bin_ss;
	string bin_str;
	bin_ss << bin;
	bin_ss>> bin_str;
	int len = bin_str.length();
	int n = 0;
	for(int i=0;i<len;++i)
	{
		if(bin_str[i]=='1')
		{
			n+=pow(2,len-1-i);
		}
	}
	return  n;
}

void PreprocessData::BoolOutput( std::vector<std::vector<int>> &sel_list, std::vector<int> &onecolumn_list, int addrMat_col_number,
	std::vector<std::vector<int>> &addrMatrix_output, std::vector<int> &dataMatrix_output_onecol)  
{
	//sel_list and onecolumn_list write into a map
	std::vector<std::vector<int>>::iterator p4;
	std::vector<int>::iterator p5;
	std::map <int, int> f_map;
	
	for (p4 = sel_list.begin(); p4 != sel_list.end(); p4++) {
		
		int *save_temp = new int[addrMat_col_number]; 
		for (p5 = p4->begin(); p5 != p4->end(); p5++) {
			size_t i = p5 - p4->begin();
			save_temp[i]= *p5;
		}

		char *buff = new char[addrMat_col_number];
		char *b = new char[addrMat_col_number];
		b[0]= '\0';
		for(int i=0; i<addrMat_col_number;i++) {
			sprintf(buff, "%d", save_temp[i]);
			strcat(b, buff);
		}
		string b_str (b);
		int b_int = atoi(b_str.c_str());
		int key = PreprocessData::BinTodec(b_int);
		f_map[key] = onecolumn_list[p4-sel_list.begin()];
	
		delete []save_temp;
		delete []buff;
		delete []b;
	}

	std::cout<<"map which has been sorted show as below: "<<std::endl;
	for(map<int,int>::iterator iter = f_map.begin(); iter != f_map.end(); ++iter) {
		std::cout<<"key:"<<iter->first<<" value:"<<iter->second<<std::endl;
	}

	//save key to vector
	std::vector<int> key_vec;
	for( std::map<int,int>::iterator it=f_map.begin(); it!=f_map.end(); it++) {   
		key_vec.push_back((*it).first);
	}
	for (int m=0; m<key_vec.size(); m++){
		//int 9 to string 1001
		string sel_row_str;
		sel_row_str = decToBin(key_vec[m], addrMat_col_number);

		std::vector<int> sel_row_temp;
		for (int s = 0; s < sel_row_str.length(); s++) {
			int int_one;
			stringstream ss_one;
			ss_one << sel_row_str[s];
			ss_one >> int_one;
			sel_row_temp.push_back(int_one);
		}
		addrMatrix_output.push_back(sel_row_temp);
		sel_row_temp.clear();
	}

	//save value to vector
	for(std::map<int,int>::iterator it2=f_map.begin(); it2!=f_map.end(); it2++) {   
		dataMatrix_output_onecol.push_back((*it2).second);
	}
	
}

// int main() {
// 	std::vector< std::vector<int> > sel_m = 
// 	{
// 		{1, 0, 1, 0},
// 		{1, 1, 1, 1},
// 		{0, 0, 1, 0},
// 		{0, 1, 1, 0},
// 		{0, 0, 0, 0},
// 		{0, 0, 1, 1},
// 		{1, 0, 1, 0},
// 	};

// 	std::vector<int> data = 
// 		{ 	12, 
// 			1, 
// 			10, 
// 			10, 
// 			5, 
// 			7, 
// 			8
// 		};
	
// 	int addrMat_col_number = sel_m[0].size();
//     std::vector<std::vector<int>> addrMatrix_output;
//     std::vector<int> dataMatrix_onecol_output;
// 	PreprocessData p;
// 	p.BoolOutput( sel_m, data, addrMat_col_number, addrMatrix_output, dataMatrix_onecol_output);
// 	for (auto iter = addrMatrix_output.begin(); iter != addrMatrix_output.end(); iter++) {
// 		vector<int> tmp = *iter;
// 		for (auto iter_ = tmp.begin(); iter_ != tmp.end(); iter_++)
// 		{
// 			cout << *iter_;
// 		}
// 		cout << endl;
// 	}

// 	for (auto iter = addrMatrix_output.begin(); iter != addrMatrix_output.end(); iter++) {
// 		vector<int> tmp = *iter;
// 		for (auto iter_ = tmp.begin(); iter_ != tmp.end(); iter_++)
// 		{
// 			cout << *iter_;
// 		}
// 		cout << endl;
// 	}

// 	for ( vector<int>::iterator iter = dataMatrix_onecol_output.begin(); iter != dataMatrix_onecol_output.end(); iter++) {
// 		cout << *iter << endl;
// 	}
// 	return 0;
// }
