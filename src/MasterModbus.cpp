/*
 * MasterModbus.cpp
 *
 *  Created on: Feb 22, 2019
 *      Author: mouses
 */

#include "MasterModbus.h"

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <modbus/modbus.h>

namespace ckr {

MasterModbus::MasterModbus() {
	ctx = modbus_new_rtu("/dev/ttyS1", 19200, 'N', 8, 1);
	//	modbus_set_debug(ctx, TRUE);
	modbus_set_error_recovery(ctx,
			modbus_error_recovery_mode(
					MODBUS_ERROR_RECOVERY_LINK
							| MODBUS_ERROR_RECOVERY_PROTOCOL));
}

MasterModbus::~MasterModbus() {
	if (ctx) {
		modbus_close(ctx);
		modbus_free(ctx);
		ctx = NULL;
	}
}

bool MasterModbus::connectSlave(int slaveAddress) {
	if (!ctx) {
		std::cerr << "Mestre inexistente";
		return false;
	}

	modbus_set_slave(ctx, 1);

	if (modbus_connect(ctx) == -1) {
		std::cerr << "Connection failed: " << modbus_strerror(errno);
//		modbus_free(ctx);
		return false;
	}

	connected = true;
	return true;
}

std::list<uint16_t> MasterModbus::getRegisters(int startAddress, int size) {
	std::list<uint16_t> ret;
	uint16_t *tab_rp_registers = (uint16_t *) malloc(size * sizeof(uint16_t));

	memset(tab_rp_registers, 0, size * sizeof(uint16_t));

	if (size == 0) {
		std::cerr << "Comando nao suportado";
		return std::list<uint16_t>();
	}

	if (!ctx) {
		std::cerr << "Mestre inexistente";
		return ret;
	}

	memset(tab_rp_registers, 0, size * sizeof(uint16_t));
	int rc = modbus_read_registers(ctx, startAddress, size, tab_rp_registers);

	if (rc == -1) {
		std::cerr << "erro ao tentativa de leitura\n";
		return ret;
	}

	for (int i = 0; i < size; ++i) {
		ret.push_back(tab_rp_registers[i]);
	}

	free(tab_rp_registers);

	return ret;
}

void MasterModbus::close(void) {
	if (ctx) {
		modbus_close(ctx);
		connected = false;
	}
}

bool MasterModbus::isConnected() const {
	return connected;
}

} /* namespace ckr */
