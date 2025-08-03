#pragma once
//STD HEADERS
#include <ostream>
#include <functional>
#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <queue>
#include <typeindex>
#include <typeinfo>
//END OF STD HEADERS
//HBR HEADERS
#include "Platform.h"
#include "Logger.h"
#include "Error.h"
#include "Core/Utils.h"
#include "Memory.h"

#define ENUMSHIFT(n) (0x1 << n)
#ifdef _DEBUG
#define DEBUG 1
#endif
//END OF HBR HEADER