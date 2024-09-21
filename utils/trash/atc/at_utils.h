/*Copyright (C) 2020 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/*
 * at_utils.h
 *
 *  Created on: 18 lug 2019
 *      Author: FabioPi, DKozenkov
 */

#ifndef HDR_AT_UTILS_H_
#define HDR_AT_UTILS_H_

#define CTRL_Z 0x1A

/*Async mode (with callback)*/
void ati_response(int cmd_id, char *resp);

M2MB_RESULT_E at_cmd_async_init(INT16 instance);
M2MB_RESULT_E at_cmd_async_deinit(void);
/*!
	@brief
		Асинхронная отправка АТ-команды
	@details
		Асинхронная отправка АТ-команды
	@param[in] *at_cmd
    	Указатель на константную строку с АТ-командой
	@param[in] *at_rsp
    	Указатель строку, которая должна содержать ответ АТ-команды
	@param[in] *at_rsp
    	Указатель строку, которая должна содержать ответ АТ-команды
	@param[in] rsp_max_len
    	Максимальное кол-во байт ответа
	@param[in] cmd_id
    	Идентификатор команды, который возвращается в виде соответствующего параметра callback-функции
    @param[in] *ati_response
    	Указатель на callback-функцию, которая будет вызвана после получения ответа АТ-команды
	@return
    	M2MB_RESULT_E код результата
  @note

  @b
    Example
  @code
    <C code example>
  @endcode
 */
M2MB_RESULT_E send_async_at_command(const CHAR *at_cmd, CHAR *at_rsp, UINT32 rsp_max_len, int cmd_id, void (*ati_response) (int, char *));


/*Sync mode (without callback)*/
M2MB_RESULT_E at_cmd_sync_init(void);
M2MB_RESULT_E at_cmd_sync_deinit(void);
M2MB_RESULT_E send_sync_at_command(const CHAR *at_cmd, CHAR *at_rsp, UINT32 rsp_max_len);
M2MB_RESULT_E send_sync_at_command_params(const CHAR *atCmd, CHAR *atRsp, UINT32 atRspMaxLen, CHAR *mask, char *params, int par_len);

#endif /* HDR_AT_UTILS_H_ */
