#include "build/test/mocks/mock_log.h"
#include "C:/Ruby26/lib/ruby/gems/2.6.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"


void setUp(void)

{

}

void tearDown(void)

{

}

void test_add( void )

{

    int result = 4;



    UnityAssertEqualNumber((UNITY_INT)((4)), (UNITY_INT)((result)), (

   ((void *)0)

   ), (UNITY_UINT)(15), UNITY_DISPLAY_STYLE_INT);

    do {if (((5) != (result))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(16)));}} while(0);

}
