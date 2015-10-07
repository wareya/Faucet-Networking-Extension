#include <cstdlib>
#include <cstdio>

#include <faucet/GmStringBuffer.hpp>

#ifdef _WIN32
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define DLLEXPORT extern "C" __attribute__((visibility("default")))
#endif

DLLEXPORT const char* mac_addrs() { // stub
	return "";
}
