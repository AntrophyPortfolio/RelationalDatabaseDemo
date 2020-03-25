#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include<stdexcept>

class NotImplementedException : public std::logic_error
{
public:
	NotImplementedException() : std::logic_error("Function is not yet implemented.") { };
};


#endif // !EXCEPTIONS_H
