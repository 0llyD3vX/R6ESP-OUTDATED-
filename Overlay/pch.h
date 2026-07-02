#pragma once

// Windows headers - included once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

// Standard libraries
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>

// Disable warnings
#pragma warning(disable: 4005) // macro redefinition
#pragma warning(disable: 4996) // deprecated functions
#pragma warning(disable: 4201) // nonstandard extension used