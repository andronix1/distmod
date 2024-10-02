#define DEBUG_MODE

#ifdef DEBUG_MODE
    #include <stdio.h>
    #define LOG_UNTAGGED(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_UNTAGGED(fmt, ...) {}
#endif

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#define LOG_LEVEL_TRACE 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL LOG_LEVEL_INFO

#define LOG_TAGGED(log_level, kind, fmt, ...) if (log_level >= LOG_LEVEL) LOG_UNTAGGED(kind "(" __FILE_NAME__ ":" STRINGIZE(__LINE__) "): " fmt, ##__VA_ARGS__)

#define LOGE(fmt, ...) LOG_TAGGED(LOG_LEVEL_ERROR, "error", fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) LOG_TAGGED(LOG_LEVEL_INFO, "info", fmt, ##__VA_ARGS__)
#define LOGT(fmt, ...) LOG_TAGGED(LOG_LEVEL_TRACE, "trace", fmt, ##__VA_ARGS__)
