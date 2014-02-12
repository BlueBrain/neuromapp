


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
    class cmech<mechanism::channel::Na>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(3,0));
            vecpair.push_back(std::make_pair<int,int>(4,1));
            vecpair.push_back(std::make_pair<int,int>(6,2));
            vecpair.push_back(std::make_pair<int,int>(7,3));
            vecpair.push_back(std::make_pair<int,int>(8,4));
            vecpair.push_back(std::make_pair<int,int>(9,5));
            vecpair.push_back(std::make_pair<int,int>(10,6));
            vecpair.push_back(std::make_pair<int,int>(11,7));
            vecpair.push_back(std::make_pair<int,int>(12,8));
            vecpair.push_back(std::make_pair<int,int>(13,9));
            vecpair.push_back(std::make_pair<int,int>(16,10));
        }

        int oldsize(){
            return 18;
        }

        int newsize(){
            return 11;
        }
    };

    template<>
    class cmech<mechanism::channel::NaTs2_t>:public cmech<mechanism::channel::Na>{
    };

    template<>
    class cmech<mechanism::channel::SKv3_1>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(3,0));
            vecpair.push_back(std::make_pair<int,int>(5,1));
            vecpair.push_back(std::make_pair<int,int>(6,2));
            vecpair.push_back(std::make_pair<int,int>(8,3));
        }

        int oldsize(){
            return 10;
        }

        int newsize(){
            return 4;
        }
    };

    template<>
    class cmech<mechanism::channel::lh>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(3,0));
            vecpair.push_back(std::make_pair<int,int>(4,1));
            vecpair.push_back(std::make_pair<int,int>(5,2));
            vecpair.push_back(std::make_pair<int,int>(6,3));
            vecpair.push_back(std::make_pair<int,int>(7,4));
            vecpair.push_back(std::make_pair<int,int>(9,5));
        }

        int oldsize(){
            return 11;
        }

        int newsize(){
            return 6;
        }
    };

    template<>
    class cmech<mechanism::channel::lm>:public base{
        public:
        cmech(){
            vecpair.push_back(std::make_pair<int,int>(3,0));
            vecpair.push_back(std::make_pair<int,int>(5,1));
            vecpair.push_back(std::make_pair<int,int>(6,2));
            vecpair.push_back(std::make_pair<int,int>(7,3));
            vecpair.push_back(std::make_pair<int,int>(8,4));
            vecpair.push_back(std::make_pair<int,int>(10,5));
        }

        int oldsize(){
            return 11;
        }

        int newsize(){
            return 6;
        }
    };

