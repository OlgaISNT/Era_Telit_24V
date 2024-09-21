#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "codec.h"
#include "app_cfg.h"
#include "files.h"
#include "log.h"
#include "params.h"
#include "m2mb_types.h"
#include "failures.h"
#include "at_command.h"
#include "azx_utils.h"
#include "m2mb_os.h"
#include "../hdr/gnss_rec.h"

static char *WHO_AM_I_CODEC = "01210000FF";
static char buffer[256];
static BOOLEAN is_rec_now = FALSE;

BOOLEAN is_rec(void){
    return is_rec_now;
}

int char2int(char input) {
	if(input >= '0' && input <= '9') return input - '0';
	if(input >= 'A' && input <= 'F') return input - 'A' + 10;
	if(input >= 'a' && input <= 'f') return input - 'a' + 10;
	return -1;
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
int hex2bin(const char* src, char* target) {
	while(src[0] && src[1]) {
		int a = char2int(src[0]);
		int b = char2int(src[1]);
		if (a < 0 || b < 0) return -1;
		*(target++) = a*16 + b;
		src += 2;
	}
	return 0;
}

int char2intT(char input) {
    if (input >= '0' && input <= '9') return input - '0';
    if (input >= 'A' && input <= 'F') return input - 'A' + 10;
    if (input >= 'a' && input <= 'f') return input - 'a' + 10;
    return -1;
}

int hex2binT(const char *src, char *target) {
    while (src[0] && src[1]) {
        int a = char2intT(src[0]);
        int b = char2intT(src[1]);
        if (a < 0 || b < 0) return -1;
        *(target++) = a * 16 + b;
        src += 2;
    }
    return 0;
}

extern char getWhoAmICodec(void){
    const int addr = 0x10;
    int i2c_fd = 0;
    memset(buffer, 0x00, 256);
    if ((i2c_fd = open("/dev/i2c-4", O_RDWR)) < 0) {
        LOG_ERROR("CODEC: Unable to open connection");
    }

    if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
        LOG_ERROR("CODEC: Unable to set slave addr");
    }

    char data[128];
    memset(data,0,128);
    if (hex2binT(WHO_AM_I_CODEC, data) < 0) {
        LOG_ERROR("CODEC: message conversion error");
    }
    int len = strlen(WHO_AM_I_CODEC)/2;
    if (write(i2c_fd, data, len) != len) {
        LOG_ERROR("CODEC: A message sending error");
    }

    INT32 readCnt = read(i2c_fd, buffer,  256);
    if(readCnt <= 0){
        LOG_ERROR("CODEC: Read error\r\n");
    }
    close(i2c_fd);
    return buffer[255];
}

UINT8 on_codec() {
	char data[200] = {0};
    if (get_audio_prof_speed() > 0) {
        if (get_gnss_data().speed >= get_audio_prof_speed()) {
        	get_nac(data, sizeof(data));
        } else {
        	get_qac(data, sizeof(data));
        }
    } else {
    	get_nac(data, sizeof(data));
    }
    BOOLEAN res = TRUE;
	//char resp[100];
	//int r = at_era_sendCommand(2*1000, resp, sizeof(resp), "AT#DVI?\r");
    //if (r == 0 || strstr(resp, "#DVI: 1,2,1") == NULL) {
    const char *req = "AT#DVI=1,2,1\r";
    res = ati_sendCommandExpectOkEx(2*1000, req); // эту команду нужно отправлять всегда после включения, даже если модем уже так сконфигурирован
    if (!res) {
    	azx_sleep_ms(1500);
    	res = ati_sendCommandExpectOkEx(2*1000, req);
    }
    //}
    if (res) return send_to_codec(data);//("0220101000242000003300340000008b"); //if (res) return send_to_codec("0220101000242000003300340000008b");
    else {
		to_log_info_uart("Can't modem DVI");
		//set_other_critical_failure(F_ACTIVE);
    	return ERR_CODEC_UNABLE_START_CODEC;
    }
}

UINT8 off_codec() {
	char data[200];
	get_codec_off(data, sizeof(data));
	return send_to_codec(data);//"02000000000000000000000000000000");
}

UINT8 send_to_codec(char *str) {
	const int addr = 0x10;
	int i2c_fd = 0;
	char data[128];
	memset(data,0,128);
	if (hex2bin(str, data) < 0) {
        set_codec_failure(F_ACTIVE);
		return ERR_CODEC_I2C_UNABLE_PARSE_HEX;
	}
	// Open a connection to the I2C userspace control file.
	if ((i2c_fd = open("/dev/i2c-4", O_RDWR)) < 0) {
		to_log_error_uart("[I2C] Unable to open i2c_4 control file");
        set_codec_failure(F_ACTIVE);
		return ERR_CODEC_I2C_UNABLE_OPEN;
	}
	if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
		to_log_error_uart("[I2C] Unable to set slave addr");
		close(i2c_fd);
        set_codec_failure(F_ACTIVE);
		return ERR_CODEC_I2C_UNABLE_SET_SLAVE;
	}
	int len = strlen(str)/2;
	LOG_DEBUG("writing to i2c %s : %d bytes", str, len);
	if (write(i2c_fd, data, len) != len) {
		to_log_error_uart("Failed to write to the i2c bus");
		close(i2c_fd);
        set_codec_failure(F_ACTIVE);
		return ERR_CODEC_I2C_UNABLE_WRITE;
	}
	close(i2c_fd);
    set_codec_failure(F_INACTIVE);
	return ERR_CODEC_NO_ERROR;
}

UINT8 play_audio(char *aud_file) {
	//char data[128];
	//memset(data,0,128);
	//sprintf(data, "AT#APLAY=1,0,\"%s\"", aud_file);
	LOG_DEBUG("Play audio file %s", aud_file);
	BOOLEAN res = ati_sendCommandExpectOkEx(2*1000, "AT#APLAY=1,0,\"%s\",%d\r", aud_file, get_spkg());
	//BOOLEAN res = ati_sendCommandExpectOkEx(3*1000, "AT#APLAY=1,0,\"%s\"\r", aud_file); // для старой прошивки модема
	if (res) return ERR_CODEC_NO_ERROR;
	else return ERR_CODEC_UNABLE_PLAY_FILE;
}

UINT8 record_audio(char *aud_file, UINT32 duration_ms) {
    is_rec_now = TRUE;
	ati_sendCommandExpectOkEx(1000, "AT#ARECD=0\r");
	char data[128];
	memset(data, 0, sizeof(data));
	sprintf(data, "/data/aplay/%s", aud_file);
	if (get_file_size(data) > -1 && !delete_file(data)) LOG_ERROR("Can't delete audio:%s", aud_file);
//	if (!ati_sendCommandExpectOkEx(3*1000, "AT#ADELF=\"%s\"\r", aud_file)) { // не всегда правильно работает
//		LOG_ERROR("Can't del %s", aud_file);
//	}
	BOOLEAN res = ati_sendCommandExpectOkEx(2*1000, "AT#ARECD=1,\"%s\"\r", aud_file);
	if (!res) {
		azx_sleep_ms(2000);
		res = ati_sendCommandExpectOkEx(2*1000, "AT#ARECD=1,\"%s\"\r", aud_file);
	}
	if (res) to_log_info_uart("Start recording audio file %s", aud_file);
	else to_log_error_uart("Can't start recording audio!");
	azx_sleep_ms(duration_ms);
	res = ati_sendCommandExpectOkEx(2*1000, "AT#ARECD=0\r");
	if (!res) {
		azx_sleep_ms(200);
		res = ati_sendCommandExpectOkEx(2*1000, "AT#ARECD=0\r");
	}
	to_log_info_uart("Stop recording audio file %s, result:%sOK", aud_file, res == 1 ? "" : "N");
	m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500)); // необходимо, чтобы иногда не возникала ошибка при последующем немедленном воспроизведении аудиофайла
    is_rec_now = FALSE;
    if (!res) return ERR_CODEC_UNABLE_STOP_RECORD_FILE;
	return ERR_CODEC_NO_ERROR;
}
