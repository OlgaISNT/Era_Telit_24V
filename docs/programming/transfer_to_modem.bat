adb push ./audio_files/by_ignition.wav /data/aplay
adb push ./audio_files/by_ignition_off.wav /data/aplay
adb push ./audio_files/by_service.wav /data/aplay
adb push ./audio_files/by_sos.wav /data/aplay
adb push ./audio_files/confirm_result.wav /data/aplay
adb push ./audio_files/critical_error.wav /data/aplay
adb push ./audio_files/ecall.wav /data/aplay
adb push ./audio_files/end_conditions.wav /data/aplay
adb push ./audio_files/end_test.wav /data/aplay
adb push ./audio_files/end_test_call.wav /data/aplay
adb push ./audio_files/error.wav /data/aplay
adb push ./audio_files/md_test.wav /data/aplay
adb push ./audio_files/no_conditions.wav /data/aplay
adb push ./audio_files/no_satellites.wav /data/aplay
adb push ./audio_files/no_test_call.wav /data/aplay
adb push ./audio_files/ok.wav /data/aplay
adb push ./audio_files/one_tone.wav /data/aplay
adb push ./audio_files/play_phrase.wav /data/aplay
adb push ./audio_files/say_phrase.wav /data/aplay
adb push ./audio_files/self_test.wav /data/aplay
adb push ./audio_files/start_test.wav /data/aplay
adb push ./audio_files/stop_test.wav /data/aplay
adb push ./audio_files/test_call.wav /data/aplay
adb push ./audio_files/two_tones.wav /data/aplay
adb push adb_credentials /var/run/adb_credentials
adb push softdog.bin /data/azc/mod/softdog.bin
adb shell chmod +x /data/azc/mod/softdog.bin
adb push era.bin /data/azc/mod
adb push factory.cfg /data/azc/mod

adb push ./dsp_files/Bluetooth_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/General_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/Global_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/Handset_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/Hdmi_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/Headset_cal.acdb /data/acdb/acdbdata
adb push ./dsp_files/Speaker_cal.acdb /data/acdb/acdbdata

PAUSE