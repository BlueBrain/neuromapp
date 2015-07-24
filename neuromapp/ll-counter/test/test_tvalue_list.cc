#include <vector>
#include "llc/tvalue_list.h"
#include "gtest/gtest.h"

using namespace llc;

template <int x>
struct vec_int_push_back {
    static void run(std::vector<int> &v) { v.push_back(x); }
};

template <unsigned x>
struct vec_unsigned_push_back {
    static void run(std::vector<unsigned> &v) { v.push_back(x); }
};

TEST(tvalue_list,int_sequence) {
    typedef tvalue_seq<int,-2,2>::type int_sequence; // -2,-1,0,1,2
    ASSERT_EQ(int_sequence::length,5);
    ASSERT_FALSE(int_sequence::empty);

    std::vector<int> values;
    int_sequence::for_each<vec_int_push_back>::run(values);
    ASSERT_EQ(values.size(),5);

    EXPECT_EQ(values[0],-2);
    EXPECT_EQ(values[1],-1);
    EXPECT_EQ(values[2],0);
    EXPECT_EQ(values[3],1);
    EXPECT_EQ(values[4],2);
}

TEST(tvalue_list,step_sequence) {
    typedef tvalue_seq<int,2,30,5>::type step_sequence; // 2,7,...,27
    ASSERT_EQ(step_sequence::length,6);
    ASSERT_FALSE(step_sequence::empty);

    std::vector<int> values;
    step_sequence::for_each<vec_int_push_back>::run(values);
    ASSERT_EQ(values.size(),6);

    EXPECT_EQ(values[0],2);
    EXPECT_EQ(values[1],7);
    EXPECT_EQ(values[5],27);
}

TEST(tvalue_list,empty_sequence) {
    typedef tvalue_seq<int,9,8>::type empty_sequence;
    ASSERT_EQ(empty_sequence::length,0);
    ASSERT_TRUE(empty_sequence::empty);

    std::vector<int> values;
    empty_sequence::for_each<vec_int_push_back>::run(values);
    EXPECT_TRUE(values.empty());
}

TEST(tvalue_list,decreasing_sequence) {
    typedef tvalue_seq<unsigned,9,3,-1>::type decreasing_sequence; // 9,8,...3
    ASSERT_EQ(decreasing_sequence::length,7);
    ASSERT_FALSE(decreasing_sequence::empty);

    std::vector<unsigned> values;
    decreasing_sequence::for_each<vec_unsigned_push_back>::run(values);
    ASSERT_EQ(values.size(),7);

    EXPECT_EQ(values[0],9);
    EXPECT_EQ(values[1],8);
    EXPECT_EQ(values.back(),3);
}

template <int p>
void add(int &x) { x+=p; }

template <void (*f)(int &)>
struct apply_intref_fn {
    static void run(int &x) { f(x); }
};

TEST(tvalue_list,fn_ptr) {
    typedef tvalue_list<void (*)(int &),add<3>,add<10>,add<20>> fn_list;
    ASSERT_EQ(fn_list::length,3);
    ASSERT_FALSE(fn_list::empty);

    int x=10;
    int expected=x+3+10+20;

    fn_list::for_each<apply_intref_fn>::run(x);
    EXPECT_EQ(x,expected);
}

template <char c>
struct char_append {
    template <typename OutIter>
    static void run(OutIter &p) { *p++=c; }
};

TEST(tvalue_list,char_list) {
    typedef tvalue_list<char,'h','e','l','l','o','\0'> char_list;
    ASSERT_EQ(char_list::length,6);
    ASSERT_FALSE(char_list::empty);

    char buffer[char_list::length];
    char *p=buffer;

    char_list::for_each<char_append>::run(p);
    ASSERT_STREQ(buffer,"hello");
}

