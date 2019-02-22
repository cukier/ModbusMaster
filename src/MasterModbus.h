/*
 * MasterModbus.h
 *
 *  Created on: Feb 22, 2019
 *      Author: mouses
 */

#ifndef MASTERMODBUS_H_
#define MASTERMODBUS_H_

#include <modbus/modbus.h>
#include <list>
#include <string>

namespace ckr {

class MasterModbus {
public:
	MasterModbus(std::string, int);
	virtual ~MasterModbus();

	bool connectSlave();
	std::list<uint16_t> getRegisters(int startAddress, int size);
	void close(void);
	bool isConnected() const;

private:
	modbus_t *ctx = NULL;
	uint16_t *tab_rp_registers = NULL;
	std::string m_porta;
	int m_slave;
	bool connected = false;
};

} /* namespace ckr */

#endif /* MASTERMODBUS_H_ */
