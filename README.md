# Grokipedia
api for grokipedia.com wikipedia by elon musk
# main
```cpp
#include "Grokipedia.h"
#include <iostream>

int main() {
   Grokipedia api;

    auto page = api.get_page("RTVI").then([](json::value result) {
        std::cout << "Search results: " << result.serialize() << std::endl;
    });
    page.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
