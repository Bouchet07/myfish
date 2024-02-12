#include "uci.h"

int main() {
    #ifdef DEBUG
    U64 b = 4546;
    #else
    UCI uci;
    uci.loop();
    return 0;
    #endif
}