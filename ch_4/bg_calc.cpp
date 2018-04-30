#include <future>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

using namespace std::chrono_literals;  // needed for 2s

int background_work(int magnitude) {

    int sum = 0;

    while (sum < magnitude) {
        sum += 1;
        
        std::this_thread::sleep_for(2ms);  // requires std::chrono_literals
    }
    
    return sum;
}

int main()
{
    std::future<int> answer = std::async(std::launch::async, 
                                         background_work,
                                         1000);  // works similar to std::thread
    
    std::vector<std::string> c_vector = {"W", "A", "I", "T"};

    for (const std::string &s:c_vector) {
        std::cout << s << std::endl;
        std::this_thread::sleep_for(2s);  // requires std::chrono_literals
    }
    
    std::cout << "The answer is " << answer.get() << std::endl; 

    return 0;
}
