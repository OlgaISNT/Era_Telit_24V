//
// Created by klokov on 28.09.2022.
//

#include "../hdr/can_util.h"
#include "factory_param.h"
#include "mcp2515.h"
#include "log.h"
#include "tasks.h"
#include "../uds/hdr/testerPresentService_3E.h"
#include "../uds/hdr/uds.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

// Cherry
static void canCherry(MESSAGE message);  // Обработчик пакетов

extern void can_cb(MESSAGE message){
    if(with_can()){ // Для блоков с CAN
        canCherry(message);
    } else {
        LOG_ERROR("CAN: Model id not set");
    }
}

static void canCherry(MESSAGE message){
    switch (message.id) {
        case 796: // 0x31C > 796
            //                             /LSB + MSB/
            //     0         1         2       >3<         4        5
            //0       7/8      15/16     23/24     31/32     39/40     47
            //1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/
            if (get_current_filter() != UDS_ONLY_TYPE_0) LOG_DEBUG(">>>>>>>>>>>>>>>> Airbags with ID 0x31C was detected");
            set_can_filter(UDS_ONLY_TYPE_0);
            airbagsCrashDetect(message.data[3]);
            goto use;
        case 800: //  0x320 > 800
            //                             /LSB + MSB/
            //     0         1         2       >3<         4        5
            //0       7/8      15/16     23/24     31/32     39/40     47
            //1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/
            if (get_current_filter() != UDS_ONLY_TYPE_1) LOG_DEBUG(">>>>>>>>>>>>>>>> Airbags with ID 0x320 was detected");
            set_can_filter(UDS_ONLY_TYPE_1);
            airbagsCrashDetect(message.data[3]);
            goto use;
        case 33:  // 0x021 > 33
            if (get_current_filter() != UDS_ONLY_TYPE_2) LOG_DEBUG(">>>>>>>>>>>>>>>> Airbags with ID 0x021 was detected");
            set_can_filter(UDS_ONLY_TYPE_2);
            //                                                 /LSB + MSB/
            //     0         1         2        3          4       >5<
            //0       7/8      15/16     23/24     31/32     39/40     47
            //1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/
            airbagsCrashDetect(message.data[5]);
            goto use;
        case 171: // 0xAB > 171
            //         /LSB + MSB/
            //     0       >1<        2         3         4         5
            //0       7/8      15/16     23/24     31/32     39/40     47
            //1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/1111 1111/
            if (get_current_filter() != UDS_ONLY_TYPE_3) LOG_DEBUG(">>>>>>>>>>>>>>>> Airbags with ID 0xAB was detected");
            set_can_filter(UDS_ONLY_TYPE_3);
            char c_byte = message.data[1];
            c_byte *= 128;
         //   LOG_DEBUG("orig=%02X content=%02X", message.data[1], c_byte);
            airbagsCrashDetect(c_byte);
            use:
               // printMessage(message, TRUE);
                if(isEnableSpecF()){
                    char str[512];
                    if(message_to_string(message, str, 512, "AIRBAGS:")){
                        save_to_file(str, strlen(str));
                    }
                }
            break;
        case UDS_DIAG_SOFT_ID_CHERRY: // Если сообщения используют ID UDS протокола
            handle_uds_request(message.data, 8);
            break;
        default:
            break;
    }
}

