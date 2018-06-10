#ifndef WALLE_COMMON_BACK_TRACE_H_
#define WALLE_COMMON_BACK_TRACE_H_
#include <cstdio>
#include <walle/common/inner_log.h>
#include <walle/config/base.h>
#include <string>

namespace walle {

void print_raw_backtrace(FILE* out = stderr, unsigned int max_frames = 63);

/*!
 * Print a plain hex stack backtrace of the called function to FILE* out,
 * prefixed with the given printf formatted output.
 */
void print_raw_backtrace(std::string &result, unsigned int max_frames,
                         const char* fmt, ...) WALLE_ATTRIBUTE_FORMAT_PRINTF(3, 4);

/*!
 * Print a demangled stack backtrace of the caller function to FILE* out.
 *
 * \warning The binary has to be compiled with <tt>-rdynamic</tt> for meaningful
 * output.
 */
void print_cxx_backtrace(FILE* out = stderr, unsigned int max_frames = 63);

// critical level
void log_back_trace();

}
#endif //WALLE_COMMON_BACK_TRACE_H_