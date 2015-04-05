#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for MAP_ANONYMOUS
#endif

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <algorithm>
#include <iterator>

#include <errno.h>    // for errno
#include <unistd.h>   // for sysconf
#include <sys/mman.h> // for mlock, munlock, madvise, mprotect, mmap, munmap, -mremap
#include <signal.h>   // for sigaction, sigemptyset, siginfo_t

namespace {

// Memory address
using Address = unsigned char*;
// Size in bytes
using Size = std::size_t;
// Blob of memory
using Blob = std::pair<Address, Size>;

// Installs a lambda as segv signal handler: prints address of violation and exits
void installSegvHandler() {
  struct ::sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  ::sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = [](int, ::siginfo_t* si, void*) {
    std::cerr << "sigsegv at address " << si->si_addr << " from " << si->si_call_addr << std::endl;
    std::exit(EXIT_FAILURE);
  };

  if (::sigaction(SIGSEGV, &sa, nullptr) == -1)
    throw std::system_error{errno, std::system_category()};
}

// Page size, 4096 is common; needed for page-aligned addresses
Size getPageSize() {
  const long pageSize = ::sysconf(_SC_PAGESIZE);
  if (pageSize == -1)
    throw std::system_error{errno, std::system_category()};
  assert(pageSize >= 1);
  return pageSize;
}

// Memory management APIs require page-aligned addresses
bool pageAligned(const Address& addr) { return reinterpret_cast<std::uintptr_t>(addr) % getPageSize() == 0; }

// Expect access, read ahead pages
void readAhead(Blob& blob) {
  assert(pageAligned(blob.first));
  if (::madvise(blob.first, blob.second, MADV_WILLNEED) == -1)
    throw std::system_error{errno, std::system_category()};
}

// Change protection flags: PROT_{NONE, READ, WRITE, EXEC}
void protect(Blob& blob, int prot) {
  assert(pageAligned(blob.first));
  if (::mprotect(blob.first, blob.second, prot) == -1)
    throw std::system_error{errno, std::system_category()};
}

// RAII wrapper marking/unmarking memory non-swapable
// Note: a locked page can not be unmaped
struct PageLockGuard final {
  PageLockGuard(const Blob& blob) : blob_{blob} {
    assert(pageAligned(blob.first));
    if (::mlock(blob_.first, blob_.second) == -1)
      throw std::system_error{errno, std::system_category()};
  }

  ~PageLockGuard() {
    const int rv = ::munlock(blob_.first, blob_.second);
    assert(rv != -1);
  }

  PageLockGuard(const PageLockGuard&) = delete;
  PageLockGuard& operator=(const PageLockGuard&) = delete;

private:
  const Blob& blob_;
};

// Anonymous (i.e. not file-backed) private (i.e. not visible to other processes) memory mapping
// Note: a locked page can not be unmaped
struct PageMapping final {
  PageMapping(Size len, int prot) {
    void* rv = ::mmap(nullptr, len, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // MAP_FAILED is (void*)-1; don't compare blob.first as its type is unsigned char*; this would invoke UB
    if (rv == MAP_FAILED)
      throw std::system_error{errno, std::system_category()};
    assert(pageAligned(static_cast<Address>(rv)));

    blob_ = {static_cast<Address>(rv), len};
    owns_ = true;
  }

  ~PageMapping() {
    if (owns_) {
      const int rv = ::munmap(blob_.first, blob_.second);
      // Page may be locked
      assert(rv != -1);
    }
  }

  PageMapping(const PageMapping&) = delete;
  PageMapping& operator=(const PageMapping&) = delete;

  PageMapping(PageMapping&& rhs) noexcept : PageMapping{} { swap(*this, rhs); }

  PageMapping& operator=(PageMapping&& rhs) & noexcept {
    swap(*this, rhs);
    return *this;
  }

  friend void swap(PageMapping& lhs, PageMapping& rhs) noexcept {
    using std::swap;
    swap(lhs.blob_, rhs.blob_);
    swap(lhs.owns_, rhs.owns_);
  }

  Blob getBlob() { return blob_; }

private:
  Blob blob_;
  bool owns_ = false;

  // For copy-swap idiom's move constructor
  PageMapping() = default;
};

} // ns


int main() try {
  // Notify me about protection violation
  installSegvHandler();

  // Writable mapping of one page
  PageMapping mapping{1u * getPageSize(), PROT_WRITE};
  auto blob = mapping.getBlob();
  // Mark mapping non-swapable
  PageLockGuard _{blob};

  std::cout << "-w- blob at " << (void*)blob.first << " size " << blob.second << std::endl;

  // This is the x86_64 machine code we want to run (see Pause.s); write into mapping -- exclude null-terminator
  const char ops[] = "\x48\x31\xc0\xb0\x22\x0f\x05";
  std::copy_n(std::begin(ops), sizeof(ops) - 1u, blob.first);

  // Until now the mapping was writable; we need it to be executable for invoking the machine code; expect access
  protect(blob, PROT_READ | PROT_EXEC);
  readAhead(blob);

  std::cout << "r-x blob at " << (void*)blob.first << " size " << blob.second << std::endl;

  // Machine code function signature
  using JitFunction = void (*)();
  JitFunction fn = reinterpret_cast<void (*)()>(blob.first);
  // Finally invoke the machine code
  fn();

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}
