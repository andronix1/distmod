#pragma once

#define LOGS_ENABLED

#ifdef LOGS_ENABLED
	#define LOG(tag, fmt, ...) printf("[" tag "]: " fmt "\n", ##__VA_ARGS__)
	#define LOGE(fmt, ...) LOG("ERROR", fmt, ##__VA_ARGS__)
#else
	#define LOGE(...)
#endif
