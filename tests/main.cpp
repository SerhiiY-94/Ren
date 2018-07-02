
#include <cstdio>

#include <math/math.hpp>

void test_anim();
void test_mat();
void test_material();
void test_mesh();
void test_program();
void test_storage();
void test_sparse_array();
void test_vec();

int main() {
    math::init();

    test_anim();
    test_mat();
    test_material();
    test_mesh();
    test_program();
    test_storage();
    test_sparse_array();
    test_vec();
    puts("OK");
}