#include "build/test/mocks/mock_log.h"
#include "hal/hdr/calc.h"
#include "C:/Ruby26/lib/ruby/gems/2.6.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"


void setUp(void)

{

}

void tearDown(void)

{

}



void log_i_CALLBACK (char* str, int cmock_num_calls) {



}



void test_add( void )

{

 log_i_CMockExpect(14, "Hi!");

 int result = calc_add(2,2);

    UnityAssertEqualNumber((UNITY_INT)((4)), (UNITY_INT)((result)), (

   ((void *)0)

   ), (UNITY_UINT)(20), UNITY_DISPLAY_STYLE_INT);

    do {if (((5) != (result))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(21)));}} while(0);

}
