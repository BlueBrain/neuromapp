//
//  statistics.h
//  neuromapp
//
//  Created by Tim Ewart on 30/01/2016.
//
//

#ifndef statistics_h
#define statistics_h

class statistic{
    public:
        explicit statistic(keyvalue::argument const& arg = keyvalue::argument(),
                           std::vector<double> vtime = std::vector<double>()):a(arg),v(vtime),g_mbw(0.),g_iops(0.){
        }

        void process(){
            double accumulate_reciprocal(0.);
            std::for_each(v.begin(), v.end(),std::bind1st(std::divides<double>(),1.)); // v[i] -> 1/v[i], for fun
            accumulate_reciprocal = keyvalue::utils::accumulate(v.begin(), v.end(), 0.); // MPI is inside
            g_iops = accumulate_reciprocal*a.cg();
            g_mbw = accumulate_reciprocal*a.voltages_size() * sizeof(double)/(1024.*1024.);
        }

        void print(std::ostream& os) const{
            std::vector<double>::const_iterator it = v.begin();
            os << "  Master only, else change the mask \n";
            
            while(it != v.end()){
                std::cout << "  Time: " << *it << "[s] \n"
                << "  I/O: "  << a.cg()/(*it) << " IOPS \n"
                << "  BW: "   << a.voltages_size()* sizeof(double)/((*it)*1024.*1024.) << " MB/s \n";
                ++it;
            }

            os << " g_iops " << g_iops << " [IOPS], " << " g_mbw "  << g_mbw << " [MB/s] \n";
        }
private:
    keyvalue::argument const & a;
    std::vector<double > v;
    double g_mbw, g_iops;
};

/** \brief basic overload the ostream operator to print the arguement */
std::ostream &operator<<(std::ostream &out, statistic&  a){
    a.print(out);
    return out;
}

#endif /* benchmark_h */
