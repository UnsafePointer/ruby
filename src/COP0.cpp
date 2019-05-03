#include "COP0.hpp"

using namespace std;

COP0::COP0() : breakPointOnExecute(0),
               breakPointOnDataAccess(0),
               jumpDestination(0),
               breakPointControl(0),
               badVirtualAddress(0),
               dataAccessBreakpointMask(0),
               executeBreakpointMask(0),
               status(),
               cause(),
               returnAddressFromTrap(0),
               processorID(0) {
}

COP0::~COP0() {

}

bool COP0::isCacheIsolated() {
    return status.isolateCache;
}

bool COP0::areInterruptsPending() {
    return (cause.interruptPending & status.interrurptMask) && status.currentInterruptEnable;
}
