#include "op_latency/primitive_ops.h"
#include "gtest/gtest.h"

void set_default_args(float &f1,float &f2,float &f3) {
    f1=1.1e-7f;
    f2=2.1f;
    f3=3.2f;
}

void set_default_args(double &f1,double &f2,double &f3) {
    f1=1.1e-7;
    f2=2.1;
    f3=3.2;
}

void set_default_args(v4float &f1,v4float &f2,v4float &f3) {
    float x1,x2,x3;
    set_default_args(x1,x2,x3);

    v4float vf1={x1,x1,x1,x1};
    v4float vf2={x2,x2,x2,x2};
    v4float vf3={x3,x3,x3,x3};

    f1=vf1;
    f2=vf2;
    f3=vf3;
}

void set_default_args(v2double &f1,v2double &f2,v2double &f3) {
    double x1,x2,x3;
    set_default_args(x1,x2,x3);

    v2double vf1={x1,x1};
    v2double vf2={x2,x2};
    v2double vf3={x3,x3};

    f1=vf1;
    f2=vf2;
    f3=vf3;
}

template <typename F>
F check_op(arith_op::arith_op op,F x1,F x2,F x3) {
    switch (op) {
    case arith_op::add:
        return x1+x2;
    case arith_op::sub:
        return x1-x2;
    case arith_op::mul:
        return x1*x2;
    case arith_op::div:
        return x1/x2;
    case arith_op::fma:
        return x1*x2+x3;
    case arith_op::sqrt:
        return std::sqrt(x1);
    case arith_op::exp:
        return std::exp(x1);
    }
}

v4float check_op(arith_op::arith_op op,v4float x1,v4float x2,v4float x3) {
    v4float r;
    for (int i=0;i<4;++i) r[i]=check_op(op,x1[i],x2[i],x3[i]);
    return r;
}

v2double check_op(arith_op::arith_op op,v2double x1,v2double x2,v2double x3) {
    v2double r;
    for (int i=0;i<2;++i) r[i]=check_op(op,x1[i],x2[i],x3[i]);
    return r;
}

void assert_fp_eq(float expected,float actual) {
    ASSERT_FLOAT_EQ(expected,actual);
}

void assert_fp_eq(double expected,double actual) {
    ASSERT_DOUBLE_EQ(expected,actual);
}

void assert_fp_eq(v4float expected,v4float actual) {
    ASSERT_FLOAT_EQ(expected[0],actual[0]);
    ASSERT_FLOAT_EQ(expected[1],actual[1]);
    ASSERT_FLOAT_EQ(expected[2],actual[2]);
    ASSERT_FLOAT_EQ(expected[3],actual[3]);
}

void assert_fp_eq(v2double expected,v2double actual) {
    ASSERT_DOUBLE_EQ(expected[0],actual[0]);
    ASSERT_DOUBLE_EQ(expected[1],actual[1]);
}

template <typename F,arith_op::arith_op op>
void run_op_test() {
    F f1,f2,f3;
    set_default_args(f1,f2,f3);

    F check=check_op(op,f1,f2,f3);
    primitive_op<op>::run(f1,f2,f3);
    assert_fp_eq(check,f1);
}

TEST(primitive_ops,add) {
    SCOPED_TRACE("add float");
    run_op_test<float,arith_op::add>();
    SCOPED_TRACE("add double");
    run_op_test<double,arith_op::add>();
    SCOPED_TRACE("add v4float");
    run_op_test<v4float,arith_op::add>();
    SCOPED_TRACE("add v2double");
    run_op_test<v2double,arith_op::add>();
}

TEST(primitive_ops,sub) {
    SCOPED_TRACE("sub float");
    run_op_test<float,arith_op::sub>();
    SCOPED_TRACE("sub double");
    run_op_test<double,arith_op::sub>();
    SCOPED_TRACE("sub v4float");
    run_op_test<v4float,arith_op::sub>();
    SCOPED_TRACE("sub v2double");
    run_op_test<v2double,arith_op::sub>();
}

TEST(primitive_ops,mul) {
    SCOPED_TRACE("mul float");
    run_op_test<float,arith_op::mul>();
    SCOPED_TRACE("mul double");
    run_op_test<double,arith_op::mul>();
    SCOPED_TRACE("mul v4float");
    run_op_test<v4float,arith_op::mul>();
    SCOPED_TRACE("mul v2double");
    run_op_test<v2double,arith_op::mul>();
}

TEST(primitive_ops,div) {
    SCOPED_TRACE("div float");
    run_op_test<float,arith_op::div>();
    SCOPED_TRACE("div double");
    run_op_test<double,arith_op::div>();
    SCOPED_TRACE("div v4float");
    run_op_test<v4float,arith_op::div>();
    SCOPED_TRACE("div v2double");
    run_op_test<v2double,arith_op::div>();
}

TEST(primitive_ops,fma) {
    SCOPED_TRACE("fma float");
    run_op_test<float,arith_op::fma>();
    SCOPED_TRACE("fma double");
    run_op_test<double,arith_op::fma>();
    SCOPED_TRACE("fma v4float");
    run_op_test<v4float,arith_op::fma>();
    SCOPED_TRACE("fma v2double");
    run_op_test<v2double,arith_op::fma>();
}

TEST(primitive_ops,sqrt) {
    SCOPED_TRACE("sqrt float");
    run_op_test<float,arith_op::sqrt>();
    SCOPED_TRACE("sqrt double");
    run_op_test<double,arith_op::sqrt>();
    SCOPED_TRACE("sqrt v4float");
    run_op_test<v4float,arith_op::sqrt>();
    SCOPED_TRACE("sqrt v2double");
    run_op_test<v2double,arith_op::sqrt>();
}

TEST(primitive_ops,exp) {
    SCOPED_TRACE("exp float");
    run_op_test<float,arith_op::exp>();
    SCOPED_TRACE("exp double");
    run_op_test<double,arith_op::exp>();
    SCOPED_TRACE("exp v4float");
    run_op_test<v4float,arith_op::exp>();
    SCOPED_TRACE("exp v2double");
    run_op_test<v2double,arith_op::exp>();
}
