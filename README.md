poison_log
==========

C++ log utilities that I usually use in my projects

##Supports
All systems with working cout. Also supports android logcat

##Requirements
- C++11
- Boost.Format
- Boost Thread (only for Android and if you've enabled synchronization)

##Usage
```cpp
#include <poison_log/log.h>

using namespace poison::utils;

int main(int argc, const char * argv[])
{
    setLogLevel(LogLevel::LOG_DEBUG);
    
    try {
        DBG("test %d", 1);
        DBG("test %d");
    } catch (const boost::io::too_few_args& e) {
        ERR("too few args");
    } catch (...) {
        ERR("unknown exception");
    }
    
    return 0;
}

```

**Output**
```
2014-09-04 15:41:13 [ D] test 1
2014-09-04 15:41:13 [ E] too few args
```

