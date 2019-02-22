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
	if (argc < 4) {
//		std::cout << argc << std::endl;
//
//		if (argc) {
//			for (int i = 0; i < argc; ++i) {
//				std::cout << argv[i] << std::endl;
//			}
//		}

		std::cout
				<< "Usage: command port_name[path] start_address qty_to_read\n";
		return -1;
	}

	int from = std::stoi(std::string(argv[2]));
	int nrs = std::stoi(std::string(argv[3]));

	ckr::MasterModbus mestre(std::string(argv[1]).c_str(), 1);

	if (!mestre.connectSlave()) {
		std::cerr << "Connection failed: " << modbus_strerror(errno);
		return -1;
	}

	for (;;) {
		fd_set fdset;
		struct timeval timeout;
		auto leit = mestre.getRegisters(from, nrs);

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
			int j = 0;

			for (auto i : leit) {
				if (j && !(j % 10)) {
					std::cout << std::endl;
				}

				std::cout << std::setw(6) << i;
				++j;
			}

			std::cout << std::endl;
			std::cout << std::endl;
		}
	}

	return -1;
}

