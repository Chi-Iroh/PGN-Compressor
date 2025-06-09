#pragma once

#ifdef TEST_MODE
    #define PRIVATE_FUNCTION
#else
    #define PRIVATE_FUNCTION static
#endif
