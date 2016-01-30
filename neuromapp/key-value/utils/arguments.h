//
//  argument.h
//  neuromapp
//
//  Created by Tim Ewart on 30/01/2016.
//
//

#ifndef argument_h
#define argument_h


class argument {
private:
    int			procs_;
    int			threads_;
    std::string	backend_;
    bool 		async_;
    bool		flash_;
    int			usecase_;
    float		st_;
    float		md_;
    float		dt_;
    int			cg_;
    int         voltages_size_;

public:
    explicit argument(int procs = 1, int threads = 1 , std::string backend = "map", bool async = false,
                 bool flash = false, int uc = 1, float st = 1., float md = 0.1, float dt = 0.025, int cg = 1) :
					procs_(procs), threads_(threads), backend_(backend), async_(async),
					flash_(flash), usecase_(uc), st_(st), md_(md), dt_(dt), cg_(cg),
                    voltages_size_(usecase_*4096/2.5*350){}
    
    argument(int argc, char * argv[]){
        std::vector<std::string> v(argv+1, argv+argc);
        argument_helper(v,"-b",backend(),to_string());
        argument_helper(v,"-st",backend(),to_double());
        argument_helper(v,"-md",backend(),to_double());
        argument_helper(v,"-dt",backend(),to_double());
        argument_helper(v,"-cg",backend(),to_int());
        argument_helper(v,"-uc",backend(),to_int());
    }

//		} else if (param == "-a") {
//			args.async() = true;
//		} else if (param == "-f") {
//			args.flash() = true;

    struct to_string{
        std::string operator()(std::string const& s){
            return s;
        }
    };

    struct to_double{
        double operator()(std::string const& s){
            return std::atof(s.c_str());
        }
    };

    struct to_int{
        double operator()(std::string const& s){
            return std::atoi(s.c_str());
        }
    };

    template<class F, class BinaryOperation>
    void argument_helper(std::vector<std::string> const& v, std::string const& s, F function, BinaryOperation op){
        std::vector<std::string>::const_iterator it;
        it = find(v.begin(), v.end(), s);
        if (it != v.end())
            function = op(*(it+1));
        else
            assert(false);
    }

    inline int voltage_size() const{
        return voltages_size_;
    }
    
    inline int procs() const {
        return procs_;
    }

    inline int threads() const {
        return threads_;
    }

    inline std::string backend() const {
        return backend_;
    }

    inline bool async() const {
        return async_;
    }

    inline bool flash() const {
        return flash_;
    }

    inline int usecase() const{
        return usecase_;
    }

    inline float st() const{
        return st_;
    }

    inline float md() const{
        return md_;
    }

    inline float dt() const{
        return dt_;
    }

    inline int cg() const{
        return cg_;
    }
    
    inline int &voltage_size(){
        return voltages_size_;
    }

    inline int &procs()  {
        return procs_;
    }

    inline int &threads()  {
        return threads_;
    }

    inline std::string &backend()  {
        return backend_;
    }

    inline bool &async()  {
        return async_;
    }

    inline bool &flash()  {
        return flash_;
    }

    inline int &usecase() {
        return usecase_;
    }

    inline float &st() {
        return st_;
    }

    inline float &md() {
        return md_;
    }

    inline float &dt() {
        return dt_;
    }

    inline int &cg() {
        return cg_;
    }
    
};


    
//    void parse_arguments(int argc, char * argv[], argvs & args){
//    std::vector<std::string> param(argv+1, argv+argc);
//
//
//    std::vector<std::string>::iterator it = find (param.begin(), param.end(), "-b");
//    if (it != param.end())
//        args.backend() = *(it+1);
//        
//    
//    std::cout << "Element found in myvector: " << *it << '\n';
//
//
//
//
//
//	for (int i = 1; i < argc; i++) {
//		std::string param(argv[i]);
//
//		if (param == "-b") {
//			args.backend() = std::string(argv[i + 1]);
//			i++;
//		} else if (param == "-a") {
//			args.async() = true;
//		} else if (param == "-f") {
//			args.flash() = true;
//
//
//    }


#endif /* argument_h */
