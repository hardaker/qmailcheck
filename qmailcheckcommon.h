#define DEBUGGING 0

#include <iostream>
#define OUTPUT(x) std::cerr << x;

#if DEBUGGING == 1
#define DEBUG(x) OUTPUT(x)
#else
#define DEBUG(x)
#endif
