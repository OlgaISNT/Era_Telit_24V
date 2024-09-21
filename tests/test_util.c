#include "util.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "unity.h"
#include "mock_log.h"
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
    TEST_ASSERT_EQUAL_INT( 30, result );
}
