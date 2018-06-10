/*
 A C++ interface to POSIX functions.

 Copyright (c) 2012 - 2016, Victor Zverovich
 All rights reserved.

 For the license information refer to format.h.
 */

// Disable bogus MSVC warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <walle/format/posix.h>
#include <walle/format/printf.h>
#include <walle/format/time.h>
#include <walle/format/string.h>
#include <walle/format/ostream.h>
#include <walle/format/posix.h>

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
# include <unistd.h>
#ifdef fileno
# undef fileno
#endif

namespace {
// Return type of read and write functions.
typedef ssize_t RWResult;

inline std::size_t convert_rwcount(std::size_t count) { return count; }
}

walle::BufferedFile::~BufferedFile() WALLE_NOEXCEPT {
  if (file_ && FMT_SYSTEM(fclose(file_)) != 0)
    walle::report_system_error(errno, "cannot close file");
}

walle::BufferedFile::BufferedFile(
    walle::CStringRef filename, walle::CStringRef mode) {
  FMT_RETRY_VAL(file_, FMT_SYSTEM(fopen(filename.c_str(), mode.c_str())), 0);
  if (!file_)
    WALLE_THROW(SystemError(errno, "cannot open file {}", filename));
}

void walle::BufferedFile::close() {
  if (!file_)
    return;
  int result = FMT_SYSTEM(fclose(file_));
  file_ = WALLE_NULL;
  if (result != 0)
    WALLE_THROW(SystemError(errno, "cannot close file"));
}

// A macro used to prevent expansion of fileno on broken versions of MinGW.
#define FMT_ARGS

int walle::BufferedFile::fileno() const {
  int fd = FMT_POSIX_CALL(fileno FMT_ARGS(file_));
  if (fd == -1)
    WALLE_THROW(SystemError(errno, "cannot get file descriptor"));
  return fd;
}

walle::File::File(walle::CStringRef path, int oflag) {
  int mode = S_IRUSR | S_IWUSR;
  FMT_RETRY(fd_, FMT_POSIX_CALL(open(path.c_str(), oflag, mode)));
  if (fd_ == -1)
    WALLE_THROW(SystemError(errno, "cannot open file {}", path));
}

walle::File::~File() WALLE_NOEXCEPT {
  // Don't retry close in case of EINTR!
  // See http://linux.derkeiler.com/Mailing-Lists/Kernel/2005-09/3000.html
  if (fd_ != -1 && FMT_POSIX_CALL(close(fd_)) != 0)
    walle::report_system_error(errno, "cannot close file");
}

void walle::File::close() {
  if (fd_ == -1)
    return;
  // Don't retry close in case of EINTR!
  // See http://linux.derkeiler.com/Mailing-Lists/Kernel/2005-09/3000.html
  int result = FMT_POSIX_CALL(close(fd_));
  fd_ = -1;
  if (result != 0)
    WALLE_THROW(SystemError(errno, "cannot close file"));
}

walle::LongLong walle::File::size() const 
{
  typedef struct stat Stat;
  Stat file_stat = Stat();
  if (FMT_POSIX_CALL(fstat(fd_, &file_stat)) == -1)
    WALLE_THROW(SystemError(errno, "cannot get file attributes"));
  WALLE_STATIC_ASSERT(sizeof(walle::LongLong) >= sizeof(file_stat.st_size),
      "return type of File::size is not large enough");
  return file_stat.st_size;
}

std::size_t walle::File::read(void *buffer, std::size_t count) {
  RWResult result = 0;
  FMT_RETRY(result, FMT_POSIX_CALL(read(fd_, buffer, convert_rwcount(count))));
  if (result < 0)
    WALLE_THROW(SystemError(errno, "cannot read from file"));
  return internal::to_unsigned(result);
}

std::size_t walle::File::write(const void *buffer, std::size_t count) {
  RWResult result = 0;
  FMT_RETRY(result, FMT_POSIX_CALL(write(fd_, buffer, convert_rwcount(count))));
  if (result < 0)
    WALLE_THROW(SystemError(errno, "cannot write to file"));
  return internal::to_unsigned(result);
}

walle::File walle::File::dup(int fd) {
  // Don't retry as dup doesn't return EINTR.
  // http://pubs.opengroup.org/onlinepubs/009695399/functions/dup.html
  int new_fd = FMT_POSIX_CALL(dup(fd));
  if (new_fd == -1)
    WALLE_THROW(SystemError(errno, "cannot duplicate file descriptor {}", fd));
  return File(new_fd);
}

void walle::File::dup2(int fd) {
  int result = 0;
  FMT_RETRY(result, FMT_POSIX_CALL(dup2(fd_, fd)));
  if (result == -1) {
    WALLE_THROW(SystemError(errno,
      "cannot duplicate file descriptor {} to {}", fd_, fd));
  }
}

void walle::File::dup2(int fd, ErrorCode &ec) WALLE_NOEXCEPT {
  int result = 0;
  FMT_RETRY(result, FMT_POSIX_CALL(dup2(fd_, fd)));
  if (result == -1)
    ec = ErrorCode(errno);
}

void walle::File::pipe(File &read_end, File &write_end) {
  // Close the descriptors first to make sure that assignments don't throw
  // and there are no leaks.
  read_end.close();
  write_end.close();
  int fds[2] = {};
  // Don't retry as the pipe function doesn't return EINTR.
  // http://pubs.opengroup.org/onlinepubs/009696799/functions/pipe.html
  int result = FMT_POSIX_CALL(pipe(fds));
  if (result != 0)
    WALLE_THROW(SystemError(errno, "cannot create pipe"));
  // The following assignments don't throw because read_fd and write_fd
  // are closed.
  read_end = File(fds[0]);
  write_end = File(fds[1]);
}

walle::BufferedFile walle::File::fdopen(const char *mode) {
  // Don't retry as fdopen doesn't return EINTR.
  FILE *f = FMT_POSIX_CALL(fdopen(fd_, mode));
  if (!f)
    WALLE_THROW(SystemError(errno, "cannot associate stream with file descriptor"));
  BufferedFile file(f);
  fd_ = -1;
  return file;
}

long walle::getpagesize() {
  long size = FMT_POSIX_CALL(sysconf(_SC_PAGESIZE));
  if (size < 0)
    WALLE_THROW(SystemError(errno, "cannot get memory page size"));
  return size;
}
