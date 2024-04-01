#include <iostream>
#include <string>

int main()
{
     std::string name = "INVITE lora #channel";


     std::string channel_name =  std::string(name.substr(name.find("#"), name.length() - name.find("#") - 1));
     std::cout << channel_name << std::endl;




}