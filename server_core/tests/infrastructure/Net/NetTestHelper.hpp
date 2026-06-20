// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetTestHelper.hpp
#pragma once

#include <chrono>
#include <thread>
#include <functional> // 引入头文件是完全正确的

namespace mmo::infrastructure::net::test
{
    // 🌟 架构级防御：确定性谓词等待
    // 🎯 修复点：将 std::functional 改为 std::function
    inline bool WaitForCondition(std::function<bool()> predicate, std::chrono::milliseconds timeout = std::chrono::milliseconds(2000))
    {
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < timeout)
        {
            if (predicate())
            {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return predicate();
    }
}