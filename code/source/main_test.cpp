#include"..\header\logger_windows.h"
#include"..\header\logger_cpp.h"
#include<iostream>
int main()
{
    DWORD dw;

    std::cout<<"logger_windows"<<std::endl;
    CFileMem filelog;
    std::string ss("C:/Bolg/code/lib_tools/tmp/logger_windows.txt");
    filelog.setLogPath(ss);
    if(filelog.Write("logger_windows",dw))
    {
        std::cout<<"file_to_log"<<std::endl;
    }
    filelog.Close();


    std::cout<<"logger_cpp"<<std::endl;
    CppLogger filelog2;
    std::string ss2("C:/Bolg/code/lib_tools/tmp/logger_cpp.txt");
    filelog2.setLogPath(ss2);
    filelog2.writeLog("logger_cpp",dw);
    filelog2.closeLog();

    getchar();
    return 0;
}