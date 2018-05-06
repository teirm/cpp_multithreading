/* Example program using a promise. */

#include <future>
#include <iostream>

void set_promise(std::promise<int>& p) {
    p.set_value(10);
}

int main() 
{
    std::promise<int> my_p;
    std::future<int> my_f = my_p.get_future();
   
    set_promise(my_p);
    int value = my_f.get();

    std::cout << value << std::endl;
    
    return 0;
}
