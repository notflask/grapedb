#include "grapedb/database.h"
#include <chrono>

int main()
{
    Grape::Database db("bench.grapedb");
    int count = 100000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; i++)
    {
        db.Set("key" + std::to_string(i), "value123");
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Writes per second: " << count / diff.count() << " ops/sec" << std::endl;
}