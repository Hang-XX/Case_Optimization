#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;
namespace Quine {
class Node{
    public:
        Node( string& term,int level);
        // ~Node();
        /**
         * Return the number of "1" in term 
         */
        int one_num();
        /**
         * Compare whether two Nodes can be merged
         */
        string getterm();
        int* compare(Node node);
        std::string term2logic();

        int covered;
        string term;

    private:
        int level;

};

class Group{
    public:
        Group(int num , std::vector<int>& lst);
        // ~group();
        /**
        * Converts the decimal num
        * to a binary string term,
        *  with insufficient zero 
        */
        std::string num2bitstr(int num);
        /**
         * Compare whether a term can cover
         *  a binary string number
         */
        int comp_bit();   
        /**
         * Group according to
         * the number of "1"
        */
        int  spilt_group();
        // string toBinary(int num);  
        void merge(int level);
        /**
         *PI List 
        */
        void Set_RIList();

        void Final_out();

        int Cmp_bin_same(const std::string& term,const std::string& num);

        void find_min_cost(std::vector<std::vector<int> >& chart,std::vector<std::vector<int>  >&QM_final);
        void find_essential_prime(std::vector<std::vector<int> >& chart,std::vector<int>& essen_prime);
        /*
        **Traverse the BFS(Breadth First Search)
        *method to find the method with the
        *fewest items
        */
        void cover_left(std::vector<std::vector<int> >& chart ,std::vector<std::vector<int>> &list_result);
        std::vector<Node> PI;
    
    private:
        int max_bits;
        std::vector<int> min_list;
        // std::vector<std::vector<Node> > node_list;
        std::vector<std::vector<std::vector<Node> >  >node_list;
};
}
