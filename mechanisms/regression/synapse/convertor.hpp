


    template<class mechanism>
    class cmech{
    };

    struct base{
        int getold(int i){
            return vecpair[i].first;
        }

        int getnew(int i){
            return vecpair[i].second;
        }
        std::vector<std::pair<int,int> > vecpair; // first int MH representation, second int new representation
    };


    template<>
    class cmech<mechanism::synapse::ProbAMPANMDA_EMS>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(0,0));
            vecpair.push_back(std::make_pair<int,int>(1,1));
            vecpair.push_back(std::make_pair<int,int>(2,2));
            vecpair.push_back(std::make_pair<int,int>(3,3));
            vecpair.push_back(std::make_pair<int,int>(22,4));
            vecpair.push_back(std::make_pair<int,int>(23,5));
            vecpair.push_back(std::make_pair<int,int>(24,6));
            vecpair.push_back(std::make_pair<int,int>(25,7));
        }

        int oldsize(){
            return 35;
        }

        int newsize(){
            return 8;
        }
    };

    template<>
    class cmech<mechanism::synapse::ProbGABAAB_EMS>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(18,0));
            vecpair.push_back(std::make_pair<int,int>(19,1));
            vecpair.push_back(std::make_pair<int,int>(20,2));
            vecpair.push_back(std::make_pair<int,int>(21,3));
            vecpair.push_back(std::make_pair<int,int>(26,4));
            vecpair.push_back(std::make_pair<int,int>(27,5));
            vecpair.push_back(std::make_pair<int,int>(28,6));
            vecpair.push_back(std::make_pair<int,int>(29,7));
        }

        int oldsize(){
            return 39;
        }

        int newsize(){
            return 8;
        }
    };
