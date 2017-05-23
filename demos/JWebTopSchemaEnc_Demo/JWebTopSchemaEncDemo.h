#pragma once
#ifndef ResourceHandlerDllExample_H
#define ResourceHandlerDllExample_H

#include "include/cef_parser.h"
extern "C" {
	_declspec(dllexport) void*  __cdecl CreateResourceHandler();
}
#endif