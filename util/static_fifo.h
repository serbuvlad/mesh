#ifndef UTIL_STATIC_FIFO_H
#define UTIL_STATIC_FIFO_H

#include <stdbool.h>
#include <stddef.h>

#define DEFINE_STATIC_FIFO(T, TZERO, N, NAME, NAME_READER_IT, NAME_WRITER_IT, HAS_F, READER_F, WRITER_F) \
static volatile T NAME[N]; \
static volatile size_t NAME_READER_IT = 0; \
static volatile size_t NAME_WRITER_IT = 0; \
\
inline static bool HAS_F(void) { \
	return NAME_READER_IT != NAME_WRITER_IT; \
} \
\
inline static T READER_F(void) { \
	if (HAS_F()) { \
		T x = NAME[NAME_READER_IT]; \
		NAME_READER_IT = (NAME_READER_IT + 1) % N; \
		return x; \
	} else { \
		return TZERO; \
	} \
} \
inline static bool WRITER_F(T x) { \
	if ((NAME_WRITER_IT + 1) % N == NAME_READER_IT) { \
		return false; \
	} else { \
		NAME[NAME_WRITER_IT] = x; \
		NAME_WRITER_IT = (NAME_WRITER_IT + 1) % N; \
		return true; \
	} \
} \

#endif // UTIL_STATIC_FIFO_H
