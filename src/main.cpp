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
	ckr::MasterModbus mestre;

	if (!mestre.connectSlave(0x01)) {
		std::cerr << "Connection failed: " << modbus_strerror(errno);
		return -1;
	}

	for (;;) {
		fd_set fdset;
		struct timeval timeout;
		auto leit = mestre.getRegisters(0, 100);

		timeout.tv_sec = 0;
		timeout.tv_usec = 150000;
		FD_ZERO(&fdset);
		FD_SET(0, &fdset);
		int rc = select(1, &fdset, NULL, NULL, &timeout);

		if (rc != 0 && rc != -1) {
			if (FD_ISSET(0, &fdset) && getchar() == 'q') {
				mestre.close();
				return 0;
			}
		}

		if (leit.size()) {
			for (auto i : leit) {
				std::cout << std::setw(6) << i;
			}

			std::cout << std::endl;
		}
	}

	return -1;
}

