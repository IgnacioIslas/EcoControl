#include "HardwareSerial.h"
/*
  This is a library written for the PCA9536 4-bit I2C I/O Expander
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14733
  Written by Jim Lindblom @ SparkFun Electronics, May 4th, 2018
  The PCA9536 is a 4-bit I/O digital expander, which communicates via an I2C bus.
  The expander can read or write four separate I/O.
  
  This library handles the initialization of the PCA9536 and supports standard
  pinMode, digitalRead, and digitalWrite functions.
  https://github.com/sparkfunX/SparkFun_PCA9536_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.5
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_PCA9536_Arduino_Library.h"

#ifdef DEBUG_PCA9536
#define PCA9536_DEBUG(x)      \
    if (_debugPort != NULL)   \
    {                         \
        _debugPort->print(x); \
    }
#define PCA9536_DEBUGLN(x)      \
    if (_debugPort != NULL)     \
    {                           \
        _debugPort->println(x); \
    }
#define STORAGE(x) (x)
#else
#define PCA9536_DEBUG(x)
#define PCA9536_DEBUGLN(x)
#define STORAGE(x) (x)
#endif

PCA9536::PCA9536()
{
    _i2cPort = NULL;
    _debugPort = NULL;
    _deviceAddress = PCA9536_ADDRESS_INVALID;
    _outputRegister = 0xF;
}

bool PCA9536::begin(TwoWire &wirePort)
{
    _deviceAddress = PCA9536_ADDRESS;
    _i2cPort = &wirePort;

    return (isConnected());
}

//Returns true if a device ack's at given address
boolean PCA9536::isConnected(void)
{
    _i2cPort->beginTransmission((uint8_t)_deviceAddress);
    return (_i2cPort->endTransmission() == 0);
}

void PCA9536::setDebugStream(Stream &debugPort)
{
    _debugPort = &debugPort;
}

PCA9536_error_t PCA9536::pinMode(uint8_t pin, uint8_t mode)
{
    PCA9536_error_t err;
    uint8_t cfgRegister = 0;

    if (pin > PCA9536_MAX_GPIO)
        return PCA9536_ERROR_UNDEFINED;

    err = readI2CRegister(&cfgRegister, PCA9536_REGISTER_CONFIGURATION);
    if (err != PCA9536_ERROR_SUCCESS)
    {
        return err;
    }
    cfgRegister &= ~(1 << pin); // Clear pin bit
    if (mode == INPUT)          // Set the bit if it's being set to INPUT (opposite of Arduino)
    {
        cfgRegister |= (1 << pin);
    }
    return writeI2CRegister(cfgRegister, PCA9536_REGISTER_CONFIGURATION);
}

PCA9536_error_t PCA9536::write(uint8_t pin, uint8_t value)
{
    uint8_t outputRegister = this->_outputRegister;
    PCA9536_error_t err;
    if (pin > PCA9536_MAX_GPIO)
        return PCA9536_ERROR_UNDEFINED;

    err = readI2CRegister((uint8_t*) &outputRegister, PCA9536_REGISTER_OUTPUT_PORT);
    if (err != PCA9536_ERROR_SUCCESS)
    {
        return err;
    }
    outputRegister = this->_outputRegister;    
    // TODO: Break out of here if it's already set correctly
    // Creamos una máscara para modificar solo el bit en la posición deseada
    uint8_t mask = 1 << pin;
    // Aplicamos la máscara al número y lo guardamos en una nueva variable
    uint8_t modified_number = outputRegister | mask;
    // Si queremos cambiar el bit a 0, aplicamos la máscara de nuevo para volver a poner el bit en 0
    if (value == HIGH) {
      modified_number = modified_number & ~mask;
    }
    this->_outputRegister=modified_number;
    return writeI2CRegister(this->_outputRegister, PCA9536_REGISTER_OUTPUT_PORT);
}

PCA9536_error_t PCA9536::digitalWrite(uint8_t pin, uint8_t value)
{
    return write(pin, value);
}

uint8_t PCA9536::readReg()
{
    PCA9536_error_t err;
    uint8_t inputRegister = 0;

    err = readI2CRegister(&inputRegister, PCA9536_REGISTER_INPUT_PORT);
    if (err != PCA9536_ERROR_SUCCESS)
    {
        return err;
    }
    return (inputRegister & (0x0f));
}

uint8_t PCA9536::read(uint8_t pin)
{
    PCA9536_error_t err;
    uint8_t inputRegister = 0;

    if (pin > PCA9536_MAX_GPIO)
        return PCA9536_ERROR_UNDEFINED;

    err = readI2CRegister(&inputRegister, PCA9536_REGISTER_INPUT_PORT);
    if (err != PCA9536_ERROR_SUCCESS)
    {
        return err;
    }
    return (inputRegister & (1 << pin)) >> pin;
}

uint8_t PCA9536::digitalRead(uint8_t pin)
{
    return read(pin);
}

PCA9536_error_t PCA9536::invert(uint8_t pin, PCA9536_invert_t inversion)
{
    PCA9536_error_t err;
    uint8_t invertRegister = 0;

    if (pin > PCA9536_MAX_GPIO)
        return PCA9536_ERROR_UNDEFINED;

    err = readI2CRegister(&invertRegister, PCA9536_REGISTER_POLARITY_INVERSION);
    if (err != PCA9536_ERROR_SUCCESS)
    {
        return err;
    }
    // TODO: Break out of here if it's already set correctly
    invertRegister &= ~(1 << pin);   // Clear pin bit
    if (inversion == PCA9536_INVERT) // Set the bit if it's being set to inverted
    {
        invertRegister |= (1 << pin);
    }
    return writeI2CRegister(invertRegister, PCA9536_REGISTER_POLARITY_INVERSION);
}

PCA9536_error_t PCA9536::revert(uint8_t pin)
{
    return invert(pin, PCA9536_RETAIN);
}

PCA9536_error_t PCA9536::readI2CBuffer(uint8_t *dest, PCA9536_REGISTER_t startRegister, uint16_t len)
{
    PCA9536_DEBUGLN((STORAGE("(readI2CBuffer): read ") + String(len) +
                     STORAGE(" @ 0x") + String(startRegister, HEX)));
    if (_deviceAddress == PCA9536_ADDRESS_INVALID)
    {
        PCA9536_DEBUGLN(STORAGE("    ERR (readI2CBuffer): Invalid address"));
        return PCA9536_ERROR_INVALID_ADDRESS;
    }
    _i2cPort->beginTransmission((uint8_t)_deviceAddress);
    _i2cPort->write(startRegister);
    if (_i2cPort->endTransmission(false) != 0)
    {
        PCA9536_DEBUGLN(STORAGE("    ERR (readI2CBuffer): End transmission"));
        return PCA9536_ERROR_READ;
    }

    _i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)len);
    for (int i = 0; i < len; i++)
    {
        dest[i] = _i2cPort->read();
        PCA9536_DEBUGLN((STORAGE("    ") + String(i) + STORAGE(": 0x") + String(dest[i], HEX)));
    }

    return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t PCA9536::writeI2CBuffer(uint8_t *src, PCA9536_REGISTER_t startRegister, uint16_t len)
{
    if (_deviceAddress == PCA9536_ADDRESS_INVALID)
    {
        PCA9536_DEBUGLN(STORAGE("ERR (readI2CBuffer): Invalid address"));
        return PCA9536_ERROR_INVALID_ADDRESS;
    }
    _i2cPort->beginTransmission((uint8_t)_deviceAddress);
    _i2cPort->write(startRegister);
    for (int i = 0; i < len; i++)
    {
        _i2cPort->write(src[i]);
    }
    if (_i2cPort->endTransmission(true) != 0)
    {
        return PCA9536_ERROR_WRITE;
    }
    return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t PCA9536::readI2CRegister(uint8_t *dest, PCA9536_REGISTER_t registerAddress)
{
    return readI2CBuffer(dest, registerAddress, 1);
}

PCA9536_error_t PCA9536::writeI2CRegister(uint8_t data, PCA9536_REGISTER_t registerAddress)
{
    return writeI2CBuffer(&data, registerAddress, 1);
}
