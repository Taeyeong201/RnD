#pragma once

#include "plog\Log.h"

#include "msquic.hpp"

//#include "QUIC_Stream.h"
//#include "QUIC_DataReceiver.h"

#ifdef PLOG
const char* StatusPrint(QUIC_STATUS status);
#else
void StatusPrint(QUIC_STATUS status);
#endif


