
#include <cstdio>

#include <math/math.hpp>

void test_anim();
void test_material();
void test_mesh();
void test_program();
void test_storage();
void test_sparse_array();

int main() {
    math::init();

    test_anim();
    test_material();
    test_mesh();
    test_program();
    test_storage();
    test_sparse_array();
    puts("OK");
}