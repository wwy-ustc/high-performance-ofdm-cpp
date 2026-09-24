#pragma once

#include <functional>


// 测量函数运行时间(ms)

double measureTime(
    std::function<void()> func
);