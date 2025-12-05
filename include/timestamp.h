#include <iomanip>
#include <string>
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdint>

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

std::time_t timestamp();

std::time_t time_parse(std::string parsed_string);

std::string timeconvert(time_t timestamp);

#endif 
