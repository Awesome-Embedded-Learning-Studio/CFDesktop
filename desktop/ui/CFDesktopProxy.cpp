#include "CFDesktop.h"
#include "CFDesktopEntity.h"

namespace cf::desktop {

CFDesktopProxy::CFDesktopProxy(aex::WeakPtr<CFDesktop> desktop) : desktop_(desktop) {}

} // namespace cf::desktop
