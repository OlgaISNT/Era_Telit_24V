#ifndef UDS_H
#define UDS_H
#include "m2mb_types.h"

//#include "msg_const.h"

// 9	Service Identifier value summary Table
#define DIAGNOSTIC_SESSION_CONTROL                  0x10 // 13.1.1	DiagnosticSessionControl service (ID 0x10)
#define ECU_RESET_SERVICE                           0x11 // 13.1.2	ECUReset Service (ID 0x11)
#define READ_DATA_BY_IDENTIFIER                     0x22
#define WRITE_DATA_BY_IDENTIFIER                    0x2E // 13.2.3	WriteDataByIdentifier Service (ID 0x2E)
#define ROUTINE_CONTROL                             0x31
#define TESTER_PRESENT                              0x3E // 13.1.54 TesterPresent Service (ID 0x3E)
#define SECURITY_ACCESS                             0x27 // 13.1.3	SecurityAccess Service (ID 0x27)
#define READ_DATA_BY_LOCAL_IDENTIFIER               0x21 // 13.2.1	ReadDataByLocalIdentifier Service (ID 0x21)
#define READ_DTC_INFORMATION                        0x19 // 13.4.2	ReadDTCInformation Service (ID 0x19)
#define INPUT_OUTPUT_CONTROL_BY_IDENTIFIER          0x2F // 13.5.1	InputOutputControlByIdentifier Service (ID 0x2F)
#define CLEAR_DIAGNOSTIC_INFORMATION                0x14 // 13.4.1	ClearDiagnosticInformation service (ID 0x14)
#define TEST_MODE_SERVICE                           0x99
#define REQUEST_DOWNLOAD_SERVICE                    0x34
#define TRANSFER_DATA_SERVICE                       0x36
#define TRANSFER_EXIT_SERVICE                       0x37
#define REQUEST_FILE_TRANSFER                       0x38

// 10	Negative Response Service ID
#define SIDRSIDNRQ                                  0x7F
// 13.1.2.4	ECUReset Positive Response
#define ERPR                                        0x51
// 13.2.2.18	ReadDataByIdentifier Positive Response ID
#define RDBIPR                                      0x62
// 13.2.3.6	WriteDataByIdentifier Positive Response ID
#define WDBIPR                                      0x6E
// routineControl Positive Response Service Id
#define RCPR                                        0x71

// 11 Supported Negative Response Codes
#define SERVICE_NOT_SUPPORTED                       0x11
#define SUBFUNCTION_NOT_SUPPORTED                   0x12
#define INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT  0x13
#define BUSY_REPEAT_REQUEST                         0x21
#define CONDITIONS_NOT_CORRECT                      0x22
#define REQUEST_SEQUENCE_ERROR                      0x24
#define requestSequenceError                        0x24
#define REQUEST_OUT_OF_RANGE                        0x31
#define SECURITY_ACCESS_DENIED                      0x33
#define invalidKey                                  0x35
#define exceededNumberOfAttempts                    0x36
#define requiredTimeDelayNotExpired                 0x37
#define generalProgrammingFailure                   0x72
#define requestCorrectlyReceived_ResponsePending    0x78
#define GENERAL_PROGRAMMING_FAILURE                 0x72
#define WRONG_BLOCK_SEQUENCE_COUNTER                0x73
#define REQUEST_SEQUENCE_ERROR                      0x24
#define TRANSFER_DATA_SUSPEND                       0x71

// 13.1.2.3	Supported resetType
#define GPS_RESET                                   0x62    // GPS reset - Cold reset и удаление информации о последнем известном местоположениии устройства

// 13.3.1.4	Supported sub-function
#define START_ROUTINE								0x01	// This parameter specifies that the server shall start the routine specified by the routineIdentifier
#define STOP_ROUTINE								0x02	// This parameter specifies that the server shall stop the routine specified by the routineIdentifier
#define REQUEST_ROUTINE_RESULTS						0x03	// This parameter specifies that the server shall return result values of the routine specified by the routineIdentifier.

//13.3.1.5	Supported routineIdentifier
#define CLEAR_ALL_MSD								0x0001	// This parameter specifies that all untransmitted MSD must be cleared
#define DELETE_FILE									0x0002	// This parameter specifies that the file containing  the saved data must be deleted
#define MODULE_REGISTRATION							0x0003	// Force the module to register in the GSM network
#define SELF_TEST									0x0004	// Begin the self-test procedure
#define BOOTSTRAP_TRANSITION						0x0005	// Switch TCM into bootstrap mode
#define SOS_CALL									0x0006	// Automatic SOS ecall procedure
#define AT_EXECUTE                                  0x0007  // Execute AT cmd
#define SOS_CALL_MANUAL                             0x0008  // Manual SOS Call
#define CLEAR_EEPROM                                0x1243  // Стирание VIN деактивация блока

//13.3.1.6	Supported modes
#define MICROPHONE_DYNAMIC							0x01	// This mode is only supported for the 0004 Self-test routineIdentifier. In this mode microphone, dynamics will be tested.
#define SOS_TEST_CALL								0x02	// This mode is only supported for the 0004 Self-test routineIdentifier. In this mode emergency call to the ECALL test number will be tested
#define SOS_TEST_CALL_NO_MTS						0x03	// This mode is only supported for the 0004 Self-test routineIdentifier. In this mode emergency call to the ECALL test number will be tested without registration on MTS
#define MICROPHONE_REC                              0x04    // Custom (only TEST_MODE)
#define MICROPHONE_PLAY                             0x05    // Custom (only TEST_MODE)

//13.3.1.9	Supported additional information about the start of the routine
#define ROUTINE_START_SUCCESS                       0x00    // It represents that the routine started to be rocessed
#define ROUTINE_START_FAILURE                       0xFF    // This value shows that the routine failed to be processed

//13.3.1.10	Supported additional information about the stop of the routine
#define ROUTINE_STOP_SUCCESS                        0x00    // It represents that the routine stoped to be rocessed
#define ROUTINE_STOP_FAILURE                        0xFF    // This value shows that the routine failed to stop to be processed

//13.3.1.11	Supported additional information routineStatusRecord  for RequestRoutineResults
#define ROUTINE_STOP                                0x00    // Routine stop or undefined
#define ROUTINE_RUN                                 0x01    // Routine running
#define ROUTINE_FINISH_ERROR                        0x02    // Routine finished with error
#define ROUTINE_FINISH_SUCCESS                      0x03    // Routine finished with no error
#define NOT_DEFINED                                 0x04    // Not defined

#define NOT_INVOLVED_BYTE                           0xAA    // Not involved byte
#define UDS_DIAG_SOFT_ID_CHERRY                     1896    // HEX 768 ID пакета диагностического ПО для протокола UDS
#define UDS_MODEM_ID_CHERY                          1912    // HEX 778 ID пакета БЭГ для протокола UDS

BOOLEAN handle_uds_request(char *data, UINT16 size);
BOOLEAN isMessage(void);
void notMessage(void);
UINT16 negative_resp(char *buf, UINT8 req_service_id, UINT8 negative_response_code);

#endif
