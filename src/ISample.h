#ifndef I_SAMPLE_H
#define I_SAMPLE_H

#include <StandardDefines.h>

DefineStandardPointers(ISample)
class ISample {
    Public Virtual ~ISample() = default;

    Public Virtual Void Perform() = 0;
};

#include "/Users/sexydevil/src/automation_src/iot_tester/src/Sample.h"
#endif // I_SAMPLE_H
