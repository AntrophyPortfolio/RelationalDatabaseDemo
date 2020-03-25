#pragma once

#ifndef SIMPLE_OPERATIONS_H
#define SIMPLE_OPERATIONS_H
#include <string>
#include "dbapi.h"

double select(std::string columnName, std::string nameOfPerson);
void update(std::string columnName, std::string nameOfPerson, double value);
std::string deletePerson(std::string name);

#endif // !SIMPLE_OPERATIONS_H
