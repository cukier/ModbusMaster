/*
 * main.cpp
 *
 *  Created on: Feb 21, 2019
 *      Author: mouses
 */

#include <sys/time.h>
#include <modbus/modbus.h>
#include <stdlib.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <thread>

#include "MasterModbus.h"

using namespace std::chrono_literals;

int main(int argc, char **argv) {
	modbus_t *ctx = NULL;
	uint16_t *tab_rp_registers = NULL;
	//	uint8_t *tab_rp_bytes = NULL;
	int nb_points = 100;
	int rc;
	fd_set fdset;
	struct timeval timeout;

	ctx = modbus_new_rtu("/dev/ttyS1", 19200, 'N', 8, 1);

	//	modbus_set_debug(ctx, TRUE);
	modbus_set_error_recovery(ctx,
			modbus_error_recovery_mode(
					MODBUS_ERROR_RECOVERY_LINK
							| MODBUS_ERROR_RECOVERY_PROTOCOL));
	modbus_set_slave(ctx, 1);

	if (modbus_connect(ctx) == -1) {
		std::cerr << "Connection failed: " << modbus_strerror(errno);
		modbus_free(ctx);
		return -1;
	}

	for (;;) {
		tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
		memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));
		rc = modbus_read_registers(ctx, 0, nb_points, tab_rp_registers);

		std::cout << "rc " << rc << '\n';

		if (rc == -1) {
			std::cerr << "erro ao tentativa de leitura\n";
			return -1;
		}

		if (rc) {
			std::cout << std::endl;

			for (int i = 0; i < rc; i++) {
				if (i && !(i % 10))
					std::cout << std::endl;

				uint8_t aux1 = (uint8_t) ((tab_rp_registers[i] & 0xFF00) >> 8);
				uint8_t aux2 = (uint8_t) tab_rp_registers[i];

				std::cout << std::setw(6) << tab_rp_registers[i];
				//				std::cout << std::setw(3) << std::string(aux1) << ' '
				//						<< std::string(aux2);
			}
		}

		std::cout << std::endl;
		free(tab_rp_registers);
		tab_rp_registers = NULL;

		timeout.tv_sec = 0;
		timeout.tv_usec = 150000;
		FD_ZERO(&fdset);
		FD_SET(0, &fdset);
		rc = select(1, &fdset, NULL, NULL, &timeout);

		if (rc != 0 && rc != -1) {
			if (FD_ISSET(0, &fdset) && getchar() == 'q') {
				modbus_close(ctx);
				modbus_free(ctx);
				return 0;
			}
		}
	}

	return -1;
}

