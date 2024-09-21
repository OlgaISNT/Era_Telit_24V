/**
 * @file test_helper.h
 * @version 1.0.0
 * @date 16.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_TEST_HELPER_H_
#define HDR_TEST_HELPER_H_

#include "m2mb_types.h"
#include "a_player.h"
#include "azx_timer.h"

void run_self_test(void);
void run_md_test(BOOLEAN only_m_d);
void play_test_call(void);
void test_confirmation_done(void);
void confirm_timer_expired(void);

#endif /* HDR_TEST_HELPER_H_ */
