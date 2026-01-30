#include <iostream>
#include <thread>
#include <random>

int main()
{
    std::default_random_engine e;
    std::uniform_real_distribution<double> d(20, 30);

    while (true)
    {
        std::cout << d(e) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
