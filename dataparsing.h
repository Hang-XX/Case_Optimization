#include <vector>

class PreprocessData {
	public:

		void BoolOutput( std::vector<std::vector<int>> &sel_list, std::vector<int> &onecolumn_list, int addrMat_col_number,
		std::vector<std::vector<int>> &addrMatrix_output, std::vector<int> &dataMatrix_output_onecol) ;

		std::string decToBin(int n, int binWidth);

		int BinTodec(int bin);

};