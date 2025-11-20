#include <iomanip>
#include <string>
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdint>

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

std::time_t timestamp();

std::string timeconvert(time_t timestamp);

#endif 
