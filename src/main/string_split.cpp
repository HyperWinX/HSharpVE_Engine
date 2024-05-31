#include <string>
#include <vector>
#include <cstdint>

void split_string(std::string& str, std::vector<std::string>& lines){
    lines.clear();
    std::string delimiter("\n");
    std::string copy(str);
    std::size_t pos = 0;
    while ((pos = copy.find(delimiter)) != std::string::npos) {
        lines.push_back(copy.substr(0, pos));
        copy.erase(0, pos + delimiter.length());
    }
    lines.push_back(copy);
}