#include "build/test/mocks/mock_log.h"
#include "C:/Ruby26/lib/ruby/gems/2.6.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"
#include "utils/util.h"
void setUp(void)

{

}



void tearDown(void)

{

}



void test_mul( void )

{

 log_i_CMockExpect(17, "mul func for: %i, %i");

 int a = 10, b = 3;

    int result = mul(&a, &b);

    UnityAssertEqualNumber((UNITY_INT)((30)), (UNITY_INT)((result)), (

   ((void *)0)

   ), (UNITY_UINT)(20), UNITY_DISPLAY_STYLE_INT);

}
