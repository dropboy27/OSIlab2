#include <iostream>
#include <sstream>
#include <string>

int main(){
    std::cout<<std::unitbuf;
    std::string curr_string;
    while(std::getline(std::cin, curr_string)){
        std::istringstream parser(curr_string);
        float num;
        float sum = 0;

        while (parser >> num) {
            sum += num;
        }
        if (parser.eof()){
            std::cout << sum << std::endl;
        }
        else{
            std::cerr<<"Error in parsing"<< std::endl;
        }
    }
    return 0;
}