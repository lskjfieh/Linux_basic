#pragma once

#include <iostream>
#include <string>
#include <stdarg.h>
#include <ctime>
//日志是有日志级别的
#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char *gLevelMap[] = {
    "DEBUG",
    "NORMAL",
    "WARNING",
    "ERROR",
    "FATAL"
};

#define LOGFILE "./threadpool.log"

//完整的日志功能，至少：日志等级 时间 日志内容 支持用户自定义
void logMessage(int level, const char *format, ...){
#ifndef DEBUG_SHOW
    if(level == DEBUG) return;
#endif
    // va_list ap;
    // va_start(ap, format); //用format初始化ap
    // while();
    // int x = va_arg(ap, int);
    // va_end(ap); // ap=nullptr


    char stdBuffer[1024]; //标准部分 （日志等级 时间）
    time_t timestamp = time(nullptr);
    // struct tm *localtime = localtime(&timestamp);
    snprintf(stdBuffer, sizeof stdBuffer, "[%s] [%ld]", gLevelMap[level], timestamp);

    char logBuffer[1024]; //自定义部分
    va_list args;
    va_start(args, format);
    // vprintf(format, args); //打到显示器
    vsnprintf(logBuffer, sizeof logBuffer, format, args);             //格式化到字符串
    va_end(args);

    // FILE *fp = fopen(LOGFILE, "a");
    printf("%s%s\n ", stdBuffer, logBuffer);
    // fprintf(fp, "%S %S\n", stdBuffer, logBuffer);
    // fclose(fp);
}