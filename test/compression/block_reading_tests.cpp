#include <iostream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

#define BOOST_TEST_MODULE devin_block_test
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

//local code for inclusion
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
using neuromapp::block;
using neuromapp::cstandard;
using namespace std;

//use raw string literals for easier creation of varied tests for read
string s1 {R"(4,5
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45)"};



string s2 {R"(10,15
208,542,532,182,73,843,24,676,393,878
444,575,499,321,896,46,973,180,184,107
359,159,976,315,103,136,183,365,84,935
417,821,604,549,914,462,880,702,773,427
56,149,60,787,113,595,997,411,606,875
201,489,287,327,777,330,441,92,113,767
334,339,850,822,832,633,394,697,4,271
448,459,327,159,365,158,24,131,389,193
613,45,148,169,13,678,61,848,903,365
338,700,491,261,245,491,330,710,490,795
626,256,969,110,55,440,999,251,864,862
50,93,214,844,914,620,863,659,739,259
637,909,502,683,175,789,270,528,205,581
248,446,309,942,144,598,773,403,994,487
618,497,955,125,230,834,894,224,828,936)"};

string s3 {R"(6,16
0.0, 0.00010101010101010101, 0.00020202020202020202, 0.00030303030303030303, 0.00040404040404040404, 0.00050505050505050505,
0.00060606060606060606, 0.00070707070707070707, 0.00080808080808080808, 0.00090909090909090909, 0.0010101010101010101, 0.0011111111111111111,
0.0012121212121212121, 0.0013131313131313131, 0.0014141414141414141, 0.0015151515151515152, 0.0016161616161616162, 0.0017171717171717172,
0.0018181818181818182, 0.0019191919191919192, 0.0020202020202020202, 0.002121212121212121, 0.0022222222222222222, 0.0023232323232323234,
0.0024242424242424242, 0.002525252525252525, 0.0026262626262626263, 0.0027272727272727275, 0.0028282828282828283, 0.0029292929292929291,
0.0030303030303030303, 0.0031313131313131315, 0.0032323232323232323, 0.0033333333333333331, 0.0034343434343434343, 0.0035353535353535356,
0.0036363636363636364, 0.0037373737373737372, 0.0038383838383838384, 0.0039393939393939396, 0.0040404040404040404, 0.0041414141414141412,
0.004242424242424242, 0.0043434343434343436, 0.0044444444444444444, 0.0045454545454545452, 0.0046464646464646469, 0.0047474747474747477,
0.0048484848484848485, 0.0049494949494949493, 0.0050505050505050501, 0.0051515151515151517, 0.0052525252525252525, 0.0053535353535353533,
0.005454545454545455, 5.5555555555555558, 0.0056565656565656566, 0.0057575757575757574, 0.0058585858585858581, 0.0059595959595959598,
0.0060606060606060606, 0.0061616161616161614, 0.0062626262626262631, 0.0063636363636363638, 0.0064646464646464646, 0.0065656565656565654,
0.0066666666666666662, 0.0067676767676767679, 0.0068686868686868687, 0.0069696969696969695, 0.0070707070707070711, 0.0071717171717171719,
0.0072727272727272727, 0.0073737373737373735, 0.0074747474747474743, 0.007575757575757576, 0.0076767676767676768, 0.0077777777777777776,
0.0078787878787878792, 0.0079797979797979791, 0.0080808080808080808, 0.0081818181818181825, 0.0082828282828282824, 0.008383838383838384,
0.008484848484848484, 0.0085858585858585856, 0.0086868686868686873, 0.0087878787878787872, 0.0088888888888888889, 0.0089898989898989905,
0.0090909090909090905, 0.0091919191919191921, 0.0092929292929292938, 0.0093939393939393937, 0.0094949494949494954, 0.0095959595959595953)"};


string s4 {""};

//error causing entries
//should throw some kind dimension error
string s5 {R"(1,5
178e2, 31006, 165e6, 18772,
2e576, e00, 10008, 23764, 23698, 26443, 2685, 6515, 28810, 13910, 20320, 19405, 31808,
A:F, EIDKL, FEEDK,CCLEECC,CEKLDE,CCDHLF,ECCDIH,FCCCEF,HICCCE,IDHDCC,HFDLCC,LLLECC,EGLJJC,CEGJKJC,
)"};

string s6 {R"(1,5
17812, 31006, 16516, 18772,
21576, 100, 10008 23764 2369826443               2685,


6515, 28810, 13910, 20320, 19405, 31808,
)"};

string s7 {R"(broken,5
17842, 31006, 16546, 18772,
24576, 400, 10008, 23764, 23698, 26443, 2685, 6515, 28810, 13910, 20320, 19405, 31808,
8303, 26189, 32218, 9220, 28912, 15932, 16530, 23564, 26151, 5319, 9992, 24977, 24787,
)"};


string s8 {R"(1,5,6,1
17842, 31006, 16546, 18772,
24576, 400, 10008, 23764, 23698, 26443, 2685, 6515, 28810, 13910, 20320, 19405, 31808,
8303, 26189, 32218, 9220, 28912, 15932, 16530, 23564, 26151, 5319, 9992, 24977, 24787,
)"};


string s9 {R"(1,5
17842, 31006, 16546, 18772,
24576, 400, 10008, 23764, 23698, 26443, 2685, 6515, 28810, 13910, 20320, 19405, 31808,
8303, 26189, 32218, 9220, 28912, 15932, 16530, 23564, 26151, 5319, 9992, 24977, 24787,
)"};

//this is a vector of the filenames that hold csv data for testing
vector<string> csv_fnames { "data/csv/values_10_a8213bulk.csv", "data/csv/values_10_a8213solo.csv", "data/csv/values_10_a8214bulk.csv", "data/csv/values_10_a8214solo.csv", "data/csv/values_10_a8215bulk.csv", "data/csv/values_10_a8215solo.csv", "data/csv/values_10_a8216bulk.csv", "data/csv/values_10_a8216solo.csv", "data/csv/values_10_a8217bulk.csv", "data/csv/values_10_a8217solo.csv", "data/csv/values_10_a8218bulk.csv", "data/csv/values_10_a8218solo.csv", "data/csv/values_10_a8219bulk.csv", "data/csv/values_10_a8219solo.csv", "data/csv/values_10_a8220bulk.csv", "data/csv/values_10_a8220solo.csv", "data/csv/values_10_a8749bulk.csv", "data/csv/values_10_a8749solo.csv", "data/csv/values_10_a8750bulk.csv", "data/csv/values_10_a8750solo.csv", "data/csv/values_10_a8751bulk.csv", "data/csv/values_10_a8751solo.csv", "data/csv/values_10_a8752bulk.csv", "data/csv/values_10_a8752solo.csv", "data/csv/values_10_a8761bulk.csv", "data/csv/values_10_a8761solo.csv", "data/csv/values_8_a10249bulk.csv", "data/csv/values_8_a10249solo.csv", "data/csv/values_8_a10250bulk.csv", "data/csv/values_8_a10250solo.csv", "data/csv/values_8_a10251bulk.csv", "data/csv/values_8_a10251solo.csv", "data/csv/values_8_a10252bulk.csv", "data/csv/values_8_a10252solo.csv", "data/csv/values_8_a10256bulk.csv", "data/csv/values_8_a10256solo.csv", "data/csv/values_8_a10261bulk.csv", "data/csv/values_8_a10261solo.csv", "data/csv/values_8_a10262bulk.csv", "data/csv/values_8_a10262solo.csv", "data/csv/values_8_a10263bulk.csv", "data/csv/values_8_a10263solo.csv", "data/csv/values_8_a10264bulk.csv", "data/csv/values_8_a10264solo.csv",  "data/csv/values_8_a8780bulk.csv", "data/csv/values_8_a8780solo.csv",  "data/csv/values_8_a8781bulk.csv", "data/csv/values_8_a8781solo.csv",  "data/csv/values_8_a8801bulk.csv", "data/csv/values_8_a8801solo.csv",  "data/csv/values_8_a8802bulk.csv", "data/csv/values_8_a8802solo.csv",  "data/csv/values_8_a8803bulk.csv", "data/csv/values_8_a8803solo.csv",  "data/csv/values_8_a8804bulk.csv", "data/csv/values_8_a8804solo.csv",  "data/csv/values_9_a10237bulk.csv", "data/csv/values_9_a10237solo.csv",  "data/csv/values_9_a10238bulk.csv", "data/csv/values_9_a10238solo.csv"};

//holder struct for combos of numeric type and allocator policy
template <class T,class A>
struct shell {
    typedef T value_type;
    typedef A allocator_type;
};

//should I be testing the align this whole time?
typedef boost::mpl::list<shell<int, neuromapp::cstandard>,
                        shell<float, neuromapp::cstandard>,
                         shell<double, neuromapp::cstandard>>
    test_allocator_types;


//TODO figure out whether this can be used somewhere in the numeric_conv process
//this is the helper function for getting the values from the string into a vector
template<typename out>
void split(const std::string &s , char delim, out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while(std::getline(ss,item,delim)) {
        *(result++) =item;       
    }
}


template<typename T>
string numeric_conv(string conv_str,T temp_converter) {
    string line,data_cell,out;
    stringstream in_ss(conv_str);
    //drop first line
    in_ss >> line;
    while(getline(in_ss,line)) {
        stringstream line_ss(line);
        //for spacing read the line outside the loop
        while(getline(line_ss,data_cell,',')) {
            stringstream data_stream(data_cell);
            data_stream >>  temp_converter;
            data_stream.str("");
            data_stream.clear();
            // effective generic type conversion
            // TODO ask tim about max precision option
            data_stream << std::setprecision(15) << temp_converter;
            out.append(data_stream.str()+ " ");
        }
        // trim the last whitespace in the line
        if (out.size() > 0) {
            out.erase((size_t) out.size()-1,(size_t) 1);
            out.append("\n");
        }
    }
    //take out the final newline in the line
    if (out.size() > 0) out.erase((size_t) out.size()-1,(size_t) 1);
    return out;
}

//this function generates the correct format for the string which we compare the block output against
//generic def
template <typename T>
string create_correct_string(string original_str) {
    std::cout << "generic version called, check typing" << std::endl;
    return string("");
}

// specialized versions for each of the numeric types used for block (int,double,float)
template <> string create_correct_string<float>(string original_str) {
    float converter;
    return numeric_conv(original_str,converter);
}

template <> string create_correct_string<double>(string original_str) {
    double converter;
    return numeric_conv(original_str,converter);
}

template <>
string create_correct_string<int>(string original_str) {
    int converter;
    return numeric_conv(original_str,converter);
}

// use lists to facilitate ease when checking various options
vector<string> test_string_vect {s1,s2,s3};
BOOST_AUTO_TEST_CASE_TEMPLATE( read_test,T,test_allocator_types) {
    //use counter to control testing on strings that have correct versions given
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    //loop through testing strings
    for (string str : test_string_vect) {
        vector<std::string> hand_made_holder;
        stringstream ss,ss2;
        ss << str;
        block<value_type,allocator_type> b1;
        //check basic error catching
        //note that the ss >> b1 will return 0 when successful
        ss >> b1;
        //capture output of block print
        // block output has no separating commas
        ss.str("");
        ss.clear();
        ss << b1;
        string correct = create_correct_string<value_type>(str);
        std::cout.precision(15);
        //std::cout << "the corrected version is\n" << correct << std::endl;
        //std::cout << "compared with block \n" << ss.str() << std::endl;
        BOOST_CHECK_MESSAGE(ss.str() == correct,
                "failed comparison: string was\n" << correct+"\n" << "block was\n" << ss.str());
        //again clear the contents of the stream
        ss.str("");
        ss.clear();
        //mem cmp test
        std::string line;
        ss<<str;
        //toss the first line
        std::getline(ss,line);
        while(std::getline(ss,line)) {
            std::stringstream line_ss(line);
            split(line_ss.str(),',',std::back_inserter(hand_made_holder));
        }
        //construct iterator for the container
        auto hand_made_it = hand_made_holder.begin();
        //built by file, now by hand for comparison
        int rows = b1.num_rows(), cols= b1.num_cols();
        block<value_type,allocator_type> b2(cols,rows);
        for (int i = 0; i < rows;i++) {
            //this is the first by hand way of checking this I could think of
            for (int j = 0;j < cols; j++) {
                std::stringstream(*hand_made_it++) >> std::dec >> b2(j,i);
            }

        }
        BOOST_CHECK(b1==b2);
    }
}


//probably a lot of tests...
BOOST_AUTO_TEST_CASE_TEMPLATE(compression_test,T,test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    std::cout << "starting compression test" << std::endl;
    for (string fname : csv_fnames) {
        std::cout << "running file " << fname << std::endl;
        ifstream ifile(fname);

        // create a block using the read
        block<value_type,allocator_type> b1;
        //make a copy of b1
        ifile>>b1;
        std::cout << "block of size" << b1.memory_allocated() << std::endl;
        block<value_type,allocator_type> b2(b1);// use the copy constructor
        //start compress
        b1.compress();
        //do the uncompress
        b1.uncompress();
        // compare the two blocks should be equal
        BOOST_CHECK(b1==b2);
    }
}



