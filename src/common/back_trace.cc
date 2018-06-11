#include <walle/common/back_trace.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

namespace walle {

void print_raw_backtrace(std::string &result, unsigned int max_frames,
                         const char* fmt, ...) 
{
    char buffer[4096];
    size_t p = 0;

    va_list args;
    va_start(args, fmt);

    p += vsnprintf(buffer + p, sizeof(buffer) - p - 1, fmt, args);

#if __linux__

    // storage array for stack trace address data
    void** addrlist = reinterpret_cast<void**>(
        alloca(sizeof(void*) * max_frames));

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, max_frames);

    for (int i = 1; i < addrlen; ++i) {
        if (addrlist[i] == nullptr)
            break;

        p += snprintf(buffer + p, sizeof(buffer) - p - 1, " %p", addrlist[i]);
    }

    buffer[p + 1] = 0;
    result = buffer;
#else

    result = "(backtrace not supported on this platform)\n";
    WALLE_UNUSED(max_frames);

#endif

    va_end(args);
}

void print_raw_backtrace(FILE* out, unsigned int max_frames) 
{
    std::string result;
    print_raw_backtrace(result, max_frames, "backtrace:");
    ::fprintf(out, "%s", result.c_str());
}

void print_cxx_backtrace(std::string &result, unsigned int max_frames) 
{
    result.append("backtrace:\n");

#if __linux__

    // storage array for stack trace address data
    void** addrlist = reinterpret_cast<void**>(
        alloca(sizeof(void*) * max_frames));

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, max_frames);

    if (addrlen == 0) {
       result += "  <empty, possibly corrupt>\n";
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = reinterpret_cast<char*>(alloca(funcnamesize));

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++) {
        char* begin_name = 0, * begin_offset = 0, * end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char* p = symbollist[i]; *p; ++p) {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
            && begin_name < begin_offset) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                result += symbollist[i];
                result += " : ";
                result += funcname;
                result += "+";
                result += begin_offset;
                result += "\n";
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                result += symbollist[i];
                result += " : ";
                result += begin_name;
                result += "+";
                result += begin_offset;
                result += "\n";
            }
        } else {
            // couldn't parse the line? print the whole line.
            //fprintf(out, "  %s\n", symbollist[i]);
            result += symbollist[i];
            result += "\n";
        }
    }

    free(symbollist);

#else

    result +=" (not supported on this platform)\n";
    WALLE_UNUSED(max_frames);

#endif
}

void log_back_trace()
{
    std::string result;
    print_cxx_backtrace(result, 63);
    LOG_CRITICAL("%s", result.c_str());
}
} //namespace walle