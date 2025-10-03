#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <cfloat>
#include <cstdint>

#include "application/logging.h"

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef __linux__
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#endif