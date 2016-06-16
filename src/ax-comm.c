#include "ax-comm.h"
#include "timing.h"
#include <wiringSerial.h>
#include <stdint.h>
#include <stdio.h>

static int serial = -1;
static long long int startTime = 0;

int initAXcomm(int baudrate) {
    serial = serialOpen("/dev/ttyAMA0", baudrate);
    if(serial == -1) {
        printf("ERROR : cannot open AX12 serial port\n");
        return -1
    }
    return 0;
}

static int axSendPacket(uint8_t id, uint8_t instruction, uint8_t command, uint8_t arg1, uint8_t arg2, int argCount) {
    uint8_t checksum = ~(id + instruction + command + arg1 + arg2 + 2 + argCount);
    if(serial < 0) {
        printf("ERROR : serial port not initialized\n");
        return -1;
    }
    serialPutchar(serial, 0xFF); serialPutchar(serial, 0xFF);
    serialPutchar(serial, id);
    serialPutchar(serial, 2+argCount);
    serialPutchar(serial, instruction);
    if(argCount>0)
        serialPutchar(serial, command);
    if(argCount>1)
        serialPutchar(serial, arg1;
    if(argCount==2)
        serialPutchar(serial, arg2);
    serialPutchar(serial, checksum);
    return 0;
}

static int checkTimeout() {
    return getCurrentTime() - startTime > 5;
}
static int axReceiveAnswer(uint8_t expectedId, uint16_t* result, uint8_t* statusError) {
    startTime = getCurrentTime();
    while(!checkTimeout()) {
        if(serialDataAvail(serial) >= 6 && serialGetchar(serial) == 0xFF && serialGetchar(serial) == 0xFF) {
            uint8_t id, length, error, checksum, arg1, arg2;
            id = serialGetchar(serial);
            length = serialGetchar(serial);
            error = serialGetchar(serial);
            if(length > 2) {
                arg1 = serialGetchar(serial);
                // wait for one more byte
                while(!checkTimeout() && serialDataAvail(serial) > 0)
                    waitForMicro(100);
            } else
                arg1 = 0;
            if(length > 2 && !checkTimeout()) {
                arg2 = serialGetchar(serial);
                // wait for one more byte
                while(!checkTimeout() && serialDataAvail(serial) > 0)
                    waitForMicro(100);
            } else
                arg2 = 0;
            // make sure packet came back complete and without error
            if(checkTimeout())
                return -4;
            checksum = serialGetchar(serial);
            if(~(id+length+error+arg1+arg2) != checksum)
                return -2;
            if(id != expectedId)
                return -3;
            if(statusError != NULL)
                *statusError = error;
            if(result != NULL)
                *result = arg1 + (arg2 << 8);
            return 0;
        }
        waitForMicro(200);
    }
    return -4;
}

int axWrite8(uint8_t id, uint8_t command, uint8_t arg, uint8_t* statusError) {
    int code;
    serialFlush(serial);
    if(axSendPacket(id, AX_WRITE_DATA, command, arg, 0, 2))
        return -1;
    if(code = axReceiveAnswer(id, NULL, statusError))
        return code;
    return 0;

}
int axWrite16(uint8_t id, uint8_t command, uint16_t arg, uint8_t* statusError) {
    int code;
    serialFlush(serial);
    if(axSendPacket(id, AX_WRITE_DATA, command, arg&0xFF, (arg >> 8)&0xFF, 3))
        return -1;
    if(code = axReceiveAnswer(id, NULL, statusError))
        return code;
    return 0;
}
int axRead8(uint8_t id, uint8_t command, uint8_t* arg, uint8_t* statusError) {
    int code;
    uint16_t arg16;
    serialFlush(serial);
    if(axSendPacket(id, AX_READ_DATA, command, 1, 0, 2))
        return -1;
    if(code = axReceiveAnswer(id, &arg16, statusError))
        return code;
    *arg = (uint8_t) arg16;
    return 0;
}
int axRead16(uint8_t id, uint8_t command, uint16_t* arg, uint8_t* statusError) {
    int code;
    serialFlush(serial);
    if(axSendPacket(id, AX_READ_DATA, command, 2, 0, 2))
        return -1;
    if(code = axReceiveAnswer(id, arg, statusError))
        return code;
    return 0;
}
