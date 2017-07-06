#include <iostream>
#include <chrono>
#include <fstream>
#include "compression.h"
#include "block.h"
#include "bit_shifting.h"
using namespace std;


using neuromapp::block;
using neuromapp::cstandard;
// todo change to variable type, and allocation method
typedef float value_type;
typedef float * pointer;
using prac_block = block<value_type,cstandard>;


vector<std::string> csv_bulk_fnames {    "../compression/trans_data/values_10_a8213trans_bulk.csv", "../compression/trans_data/values_10_a8214trans_bulk.csv", "../compression/trans_data/values_10_a8215trans_bulk.csv", "../compression/trans_data/values_10_a8216trans_bulk.csv", "../compression/trans_data/values_10_a8217trans_bulk.csv", "../compression/trans_data/values_10_a8218trans_bulk.csv", "../compression/trans_data/values_10_a8219trans_bulk.csv", "../compression/trans_data/values_10_a8220trans_bulk.csv", "../compression/trans_data/values_10_a8749trans_bulk.csv", "../compression/trans_data/values_10_a8750trans_bulk.csv", "../compression/trans_data/values_10_a8751trans_bulk.csv", "../compression/trans_data/values_10_a8752trans_bulk.csv", "../compression/trans_data/values_10_a8761trans_bulk.csv", "../compression/trans_data/values_8_a10249trans_bulk.csv", "../compression/trans_data/values_8_a10250trans_bulk.csv", "../compression/trans_data/values_8_a10251trans_bulk.csv", "../compression/trans_data/values_8_a10252trans_bulk.csv", "../compression/trans_data/values_8_a10256trans_bulk.csv", "../compression/trans_data/values_8_a10261trans_bulk.csv", "../compression/trans_data/values_8_a10262trans_bulk.csv", "../compression/trans_data/values_8_a10263trans_bulk.csv", "../compression/trans_data/values_8_a10264trans_bulk.csv", "../compression/trans_data/values_8_a8780trans_bulk.csv", "../compression/trans_data/values_8_a8781trans_bulk.csv", "../compression/trans_data/values_8_a8801trans_bulk.csv", "../compression/trans_data/values_8_a8802trans_bulk.csv", "../compression/trans_data/values_8_a8803trans_bulk.csv", "../compression/trans_data/values_8_a8804trans_bulk.csv", "../compression/trans_data/values_9_a10237trans_bulk.csv", "../compression/trans_data/values_9_a10238trans_bulk.csv", "../compression/trans_data/values_9_a10239trans_bulk.csv", "../compression/trans_data/values_9_a10240trans_bulk.csv", "../compression/trans_data/values_9_a10245trans_bulk.csv", "../compression/trans_data/values_9_a10257trans_bulk.csv", "../compression/trans_data/values_9_a10258trans_bulk.csv", "../compression/trans_data/values_9_a10259trans_bulk.csv", "../compression/trans_data/values_9_a10260trans_bulk.csv", "../compression/trans_data/values_9_a513trans_bulk.csv", "../compression/trans_data/values_9_a514trans_bulk.csv", "../compression/trans_data/values_9_a515trans_bulk.csv", "../compression/trans_data/values_9_a516trans_bulk.csv", "../compression/trans_data/values_9_a8737trans_bulk.csv", "../compression/trans_data/values_9_a8738trans_bulk.csv", "../compression/trans_data/values_9_a8739trans_bulk.csv", "../compression/trans_data/values_9_a8740trans_bulk.csv", "../compression/trans_data/values_9_a8782trans_bulk.csv", "../compression/trans_data/values_9_a8783trans_bulk.csv", "../compression/trans_data/values_9_a8784trans_bulk.csv", "../compression/trans_data/values_9_a8785trans_bulk.csv", "../compression/trans_data/values_9_a8786trans_bulk.csv", "../compression/trans_data/values_9_a8787trans_bulk.csv", "../compression/trans_data/values_9_a8788trans_bulk.csv", "../compression/trans_data/values_9_a8789trans_bulk.csv", "../compression/trans_data/values_9_a8790trans_bulk.csv", "../compression/trans_data/values_9_a8791trans_bulk.csv", "../compression/trans_data/values_9_a8792trans_bulk.csv", "../compression/trans_data/values_9_a8825trans_bulk.csv", "../compression/trans_data/values_9_a8826trans_bulk.csv", "../compression/trans_data/values_9_a8827trans_bulk.csv", "../compression/trans_data/values_9_a8828trans_bulk.csv" };


int main (void) {
    ofstream out("res.log");

    for (string fname : csv_bulk_fnames) {
        out << fname;
        prac_block b1;
        ifstream ifile(fname);
        ifile >> b1;
        // not actually limited to the row otherwise
        size_type cols_ = b1.dim0(), rows_ = b1.num_rows();
        out << "elements : "<< (cols_*rows_) << "\t";
        // copy has 3times as many columns and same number of rows
        block<unsigned int, cstandard> split_block(cols_*3,rows_);
        // create output stream for time logging
        chrono::time_point<chrono::system_clock> start,end;
        start = chrono::system_clock::now();
        neuromapp::populate_split_block(split_block,b1);
        end = chrono::system_clock::now();
        chrono::duration<double,std::milli> time_for_sep = end-start;
        out << "separation took: " << time_for_sep.count() << "\n";
        //recombine the split block, and compare with the original
        prac_block b1_comp = neuromapp::generate_unsplit_block(split_block);
        if ( b1_comp == b1) {
            out << "yup they compared as equal" << std::endl;
        } else {
            out << "nope, weren't equal" << std::endl;
            out << "b1 is " << b1;
            out << "b1 comp is " << b1_comp;
        }
    }
}
