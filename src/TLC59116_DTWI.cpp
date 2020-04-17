/*
 * Copyright (c) 2017, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 
 *  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <TLC59116_DTWI.h>

const uint8_t TLC59116_DTWI::pinmap[8] = {
    10 << 4 | 14,
    11 << 4 | 15,
    5 << 4 | 1,
    6 << 4 | 2,
    7 << 4 | 3,
    9 << 4 | 13,
    8 << 4 | 12,
    4 << 4 | 0
};

const uint8_t TLC59116_DTWI::numbers[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00100111,
    0b01111111,
    0b01101111
};

void TLC59116_DTWI::begin() {
    if (_begun == 0) {
        _dtwi->beginMaster(DTWI::FQ400KHz);
        writeRegister(TLC59116_MODE1, 0x01);
        delay(1);
        writeRegister(TLC59116_MODE2, 0x00);
        writeRegister(TLC59116_LEDOUT0, 0b10101010);
        writeRegister(TLC59116_LEDOUT1, 0b10101010);
        writeRegister(TLC59116_LEDOUT2, 0b10101010);
        writeRegister(TLC59116_LEDOUT3, 0b10101010);
        this->analogWrite(0, 0);
        this->analogWrite(1, 0);
        this->analogWrite(2, 0);
        this->analogWrite(3, 0);
        this->analogWrite(4, 0);
        this->analogWrite(5, 0);
        this->analogWrite(6, 0);
        this->analogWrite(7, 0);
        this->analogWrite(8, 0);
        this->analogWrite(9, 0);
        this->analogWrite(10, 0);
        this->analogWrite(11, 0);
        this->analogWrite(12, 0);
        this->analogWrite(13, 0);
        this->analogWrite(14, 0);
        this->analogWrite(15, 0);
        setPinMapping(pinmap);
    }
    _begun = 1;
}


void TLC59116_DTWI::writeRegister(uint8_t reg, uint8_t val) {
    uint8_t state = 0;
    uint32_t ts = millis();
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
            return;
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(TLC59116_BASEADDR | (_addr & 0x0F))) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(&reg, 1) == 1) {
                    state = 2;
                }
                break;
            case 2: // Send register address
                if (_dtwi->write(&val, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: // Stop Master
                if (_dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

void TLC59116_DTWI::analogWrite(uint8_t chan, uint8_t b) {
    writeRegister(TLC59116_PWM0 + (chan & 0x0F), b);
}

void TLC59116_DTWI::displayNumber(uint8_t number, uint8_t b) {

    uint8_t tens = (number / 10) % 10;
    uint8_t units = number % 10;

    if (tens == 0 && !_leading) {
        for (int i = 0; i < 8; i++) {
            int seg = _currentPinMapping[i] >> 4;
            this->analogWrite(seg, 0);
        }
    } else {
        int num = numbers[tens];
        for (int i = 0; i < 8; i++) {
            int seg = _currentPinMapping[i] >> 4;
            if (num & (1<<i)) {
                this->analogWrite(seg, b);
            } else {
                this->analogWrite(seg, 0);
            }
        }
    }

    int num = numbers[units];
    for (int i = 0; i < 8; i++) {
        int seg = _currentPinMapping[i] & 0x0F;
        if (num & (1<<i)) {
            this->analogWrite(seg, b);
        } else {
            this->analogWrite(seg, 0);
        }
    }

    this->analogWrite(_currentPinMapping[7] & 0x0F, _dp & 0x01 ? b : 0);
    this->analogWrite(_currentPinMapping[7] >> 4, _dp & 0x02 ? b : 0);

}

void TLC59116_DTWI::setPinMapping(const uint8_t *map) {
    _currentPinMapping = map;
}
