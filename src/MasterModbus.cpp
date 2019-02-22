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

MasterModbus::MasterModbus(std::string porta, int slave) :
		m_porta(porta), m_slave(slave) {
	ctx = modbus_new_rtu(porta.c_str(), 19200, 'N', 8, 1);
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
		std::cout << "memoria limpa" << std::endl;
	}
}

bool MasterModbus::connectSlave() {
	if (!ctx) {
		std::cerr << "Mestre inexistente";
		return false;
	}

	modbus_set_slave(ctx, m_slave);

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
	uint16_t *tab_rp_registers;
	int rc = -1;

	if (size == 0) {
		std::cerr << "Comando nao suportado";
		return std::list<uint16_t>();
	}

	if (!ctx) {
		std::cerr << "Mestre inexistente";
		return ret;
	}

	if (size < 125) {
		tab_rp_registers = (uint16_t *) malloc(size * sizeof(uint16_t));
		memset(tab_rp_registers, 0, size * sizeof(uint16_t));
		rc = modbus_read_registers(ctx, startAddress, size, tab_rp_registers);

		if (rc == -1) {
			std::cerr << "erro ao tentativa de leitura\n";
			return ret;
		}

		for (int i = 0; i < size; ++i) {
			ret.push_back(tab_rp_registers[i]);
		}
	} else {
		tab_rp_registers = (uint16_t *) malloc(125 * sizeof(uint16_t));
		memset(tab_rp_registers, 0, 125 * sizeof(uint16_t));

		int until = int((float) size / 125);

		for (int c = 0; c <= until; ++c) {
			int to = 0;

			if (c == until - 1) {
				to = 125;
			} else {
				to = int(size % 125);
			}

			int sa = (c * 125) + startAddress;

			rc = modbus_read_registers(ctx, sa, to, tab_rp_registers);

			for (int i = 0; i < to; ++i) {
				ret.push_back(tab_rp_registers[i]);
			}
		}
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
