#include "calcEditDist.h"
#include <string>
int main(){
    std::string A,B;
    A = "abcde";
    B = "abcde"; 
    printf("len=%d\n",chcpy::calcEditDist(A,B));
    return 0;
}