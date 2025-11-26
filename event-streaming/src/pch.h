#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "application/logging.h"

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif