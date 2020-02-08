#include "llvm/Pass.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <set>

// string to move the cursor, in order to print tree shape
#define MOVEUP(x) errs() << "\033[" << (x) << "A"
#define MOVEDOWN(x) errs() << "\033[" << (x) << "B"
#define MOVEDLEFT(x) errs() << "\033[" << (x) << "D"
#define MOVEDRIGHT(x) errs() << "\033[" << (x) << "C"

// decide if set the duplicate remove
#define REMOVE_DUPLICATE true

// set the minimum sum of instruction, to measure the call function whether to be search
#define MIN_INS_NUM 0

// set the search depth
#define MAX_DEPTH 5

using namespace llvm;

namespace {
    struct FindFunc : public FunctionPass {
        static char ID;
        // store the syscall function name to be searched, initialized in the constructor
        std::set<std::string> syscall_set;

        FindFunc() : FunctionPass(ID) {
            std::string head = "gvisor.x2edev..z2fgvisor..z2fpkg..z2fsentry..z2fsyscalls..z2flinux.";
            // fully support
            syscall_set.insert(head + "Read");
            syscall_set.insert(head + "Write");
            syscall_set.insert(head + "Close");
            syscall_set.insert(head + "Stat");
            syscall_set.insert(head + "Fstat");
            syscall_set.insert(head + "Lstat");
            syscall_set.insert(head + "Poll");
            syscall_set.insert(head + "Lseek");
            syscall_set.insert(head + "Mprotect");
            syscall_set.insert(head + "Munmap");
            syscall_set.insert(head + "Brk");
            syscall_set.insert(head + "RtSigaction");
            syscall_set.insert(head + "RtSigprocmask");
            syscall_set.insert(head + "RtSigreturn");
            syscall_set.insert(head + "Pread64");
            syscall_set.insert(head + "Pwrite64");
            syscall_set.insert(head + "Readv");
            syscall_set.insert(head + "Writev");
            syscall_set.insert(head + "Access");
            syscall_set.insert(head + "Pipe");
            syscall_set.insert(head + "Select");
            syscall_set.insert(head + "SchedYield");
            syscall_set.insert(head + "Mremap");
            syscall_set.insert(head + "Dup");
            syscall_set.insert(head + "Dup2");
            syscall_set.insert(head + "Pause");
            syscall_set.insert(head + "Nanosleep");
            syscall_set.insert(head + "Getitimer");
            syscall_set.insert(head + "Alarm");
            syscall_set.insert(head + "Setitimer");
            syscall_set.insert(head + "Getpid");
            syscall_set.insert(head + "Sendfile");
            syscall_set.insert(head + "Connect");
            syscall_set.insert(head + "Accept");
            syscall_set.insert(head + "SendTo");
            syscall_set.insert(head + "RecvFrom");
            syscall_set.insert(head + "SendMsg");
            syscall_set.insert(head + "Listen");
            syscall_set.insert(head + "GetSockName");
            syscall_set.insert(head + "GetPeerName");
            syscall_set.insert(head + "SocketPair");
            syscall_set.insert(head + "Fork");
            syscall_set.insert(head + "Vfork");
            syscall_set.insert(head + "Execve");
            syscall_set.insert(head + "Exit");
            syscall_set.insert(head + "Wait4");
            syscall_set.insert(head + "Kill");
            syscall_set.insert(head + "Uname");
            syscall_set.insert(head + "Semget");
            syscall_set.insert(head + "Shmdt");
            syscall_set.insert(head + "Truncate");
            syscall_set.insert(head + "Ftruncate");
            syscall_set.insert(head + "Getdents");
            syscall_set.insert(head + "Getcwd");
            syscall_set.insert(head + "Chdir");
            syscall_set.insert(head + "Fchdir");
            syscall_set.insert(head + "Rename");
            syscall_set.insert(head + "Mkdir");
            syscall_set.insert(head + "Rmdir");
            syscall_set.insert(head + "Creat");
            syscall_set.insert(head + "Link");
            syscall_set.insert(head + "Unlink");
            syscall_set.insert(head + "Symlink");
            syscall_set.insert(head + "Readlink");
            syscall_set.insert(head + "Chmod");
            syscall_set.insert(head + "Chown");
            syscall_set.insert(head + "Fchown");
            syscall_set.insert(head + "Lchown");
            syscall_set.insert(head + "Umask");
            syscall_set.insert(head + "Gettimeofday");
            syscall_set.insert(head + "Getrlimit");
            syscall_set.insert(head + "Times");
            syscall_set.insert(head + "Getuid");
            syscall_set.insert(head + "Getgid");
            syscall_set.insert(head + "Setuid");
            syscall_set.insert(head + "Setgid");
            syscall_set.insert(head + "Geteuid");
            syscall_set.insert(head + "Getegid");
            syscall_set.insert(head + "Setpgid");
            syscall_set.insert(head + "Getppid");
            syscall_set.insert(head + "Getpgrp");
            syscall_set.insert(head + "Setsid");
            syscall_set.insert(head + "Setreuid");
            syscall_set.insert(head + "Setregid");
            syscall_set.insert(head + "Getgroups");
            syscall_set.insert(head + "Setgroups");
            syscall_set.insert(head + "Setresuid");
            syscall_set.insert(head + "Getresuid");
            syscall_set.insert(head + "Setresgid");
            syscall_set.insert(head + "Getresgid");
            syscall_set.insert(head + "Getpgid");
            syscall_set.insert(head + "Getsid");
            syscall_set.insert(head + "Capget");
            syscall_set.insert(head + "Capset");
            syscall_set.insert(head + "RtSigpending");
            syscall_set.insert(head + "RtSigtimedwait");
            syscall_set.insert(head + "RtSigqueueinfo");
            syscall_set.insert(head + "RtSigsuspend");
            syscall_set.insert(head + "Sigaltstack");
            syscall_set.insert(head + "Utime");
            syscall_set.insert(head + "Chroot");
            syscall_set.insert(head + "Sethostname");
            syscall_set.insert(head + "Setdomainname");
            syscall_set.insert(head + "Gettid");
            syscall_set.insert(head + "Readahead");
            syscall_set.insert(head + "Tkill");
            syscall_set.insert(head + "Time");
            syscall_set.insert(head + "EpollCreate");
            syscall_set.insert(head + "Getdents64");
            syscall_set.insert(head + "SetTidAddress");
            syscall_set.insert(head + "RestartSyscall");
            syscall_set.insert(head + "TimerCreate");
            syscall_set.insert(head + "TimerSettime");
            syscall_set.insert(head + "TimerGettime");
            syscall_set.insert(head + "TimerGetoverrun");
            syscall_set.insert(head + "TimerDelete");
            syscall_set.insert(head + "ClockSettime");
            syscall_set.insert(head + "ClockGettime");
            syscall_set.insert(head + "ClockGetres");
            syscall_set.insert(head + "ClockNanosleep");
            syscall_set.insert(head + "ExitGroup");
            syscall_set.insert(head + "EpollWait");
            syscall_set.insert(head + "EpollCtl");
            syscall_set.insert(head + "Tgkill");
            syscall_set.insert(head + "Utimes");
            syscall_set.insert(head + "Waitid");
            syscall_set.insert(head + "Openat");
            syscall_set.insert(head + "Mkdirat");
            syscall_set.insert(head + "Mknodat");
            syscall_set.insert(head + "Fchownat");
            syscall_set.insert(head + "Futimesat");
            syscall_set.insert(head + "Fstatat");
            syscall_set.insert(head + "Unlinkat");
            syscall_set.insert(head + "Renameat");
            syscall_set.insert(head + "Linkat");
            syscall_set.insert(head + "Symlinkat");
            syscall_set.insert(head + "Readlinkat");
            syscall_set.insert(head + "Fchmodat");
            syscall_set.insert(head + "Faccessat");
            syscall_set.insert(head + "Pselect");
            syscall_set.insert(head + "Ppoll");
            syscall_set.insert(head + "Splice");
            syscall_set.insert(head + "Tee");
            syscall_set.insert(head + "Utimensat");
            syscall_set.insert(head + "EpollPwait");
            syscall_set.insert(head + "TimerfdCreate");
            syscall_set.insert(head + "Eventfd");
            syscall_set.insert(head + "TimerfdSettime");
            syscall_set.insert(head + "TimerfdGettime");
            syscall_set.insert(head + "Accept4");
            syscall_set.insert(head + "Eventfd2");
            syscall_set.insert(head + "EpollCreate1");
            syscall_set.insert(head + "Dup3");
            syscall_set.insert(head + "Pipe2");
            syscall_set.insert(head + "InotifyInit1");
            syscall_set.insert(head + "Preadv");
            syscall_set.insert(head + "Pwritev");
            syscall_set.insert(head + "RtTgsigqueueinfo");
            syscall_set.insert(head + "Prlimit64");
            syscall_set.insert(head + "Getcpu");
            syscall_set.insert(head + "Seccomp");
            syscall_set.insert(head + "GetRandom");
            syscall_set.insert(head + "MemfdCreate");
            syscall_set.insert(head + "Execveat");
            syscall_set.insert(head + "Preadv2");
            syscall_set.insert(head + "Statx");

            // Partially support
            syscall_set.insert(head + "Open");
            syscall_set.insert(head + "Mmap");
            syscall_set.insert(head + "Ioctl");
            syscall_set.insert(head + "Msync");
            syscall_set.insert(head + "Mincore");
            syscall_set.insert(head + "Madvise");
            syscall_set.insert(head + "Shmget");
            syscall_set.insert(head + "Shmat");
            syscall_set.insert(head + "Shmctl");
            syscall_set.insert(head + "Socket");
            syscall_set.insert(head + "RecvMsg");
            syscall_set.insert(head + "Shutdown");
            syscall_set.insert(head + "Bind");
            syscall_set.insert(head + "SetSockOpt");
            syscall_set.insert(head + "GetSockOpt");
            syscall_set.insert(head + "Clone");
            syscall_set.insert(head + "Semop");
            syscall_set.insert(head + "Semctl");
            syscall_set.insert(head + "Fcntl");
            syscall_set.insert(head + "Flock");
            syscall_set.insert(head + "Fsync");
            syscall_set.insert(head + "Fdatasync");
            syscall_set.insert(head + "Fchmod");
            syscall_set.insert(head + "Getrusage");
            syscall_set.insert(head + "Sysinfo");
            syscall_set.insert(head + "Ptrace");
            syscall_set.insert(head + "Syslog");
            syscall_set.insert(head + "Mknod");
            syscall_set.insert(head + "Statfs");
            syscall_set.insert(head + "Fstatfs");
            syscall_set.insert(head + "Getpriority");
            syscall_set.insert(head + "Setpriority");
            syscall_set.insert(head + "SchedGetparam");
            syscall_set.insert(head + "SchedSetscheduler");
            syscall_set.insert(head + "SchedGetscheduler");
            syscall_set.insert(head + "SchedGetPriorityMax");
            syscall_set.insert(head + "SchedGetPriorityMin");
            syscall_set.insert(head + "Mlock");
            syscall_set.insert(head + "Munlock");
            syscall_set.insert(head + "Mlockall");
            syscall_set.insert(head + "Munlockall");
            syscall_set.insert(head + "Prctl");
            syscall_set.insert(head + "ArchPrctl");
            syscall_set.insert(head + "Setrlimit");
            syscall_set.insert(head + "Sync");
            syscall_set.insert(head + "Mount");
            syscall_set.insert(head + "Umount2");
            syscall_set.insert(head + "Setxattr");
            syscall_set.insert(head + "Getxattr");
            syscall_set.insert(head + "Futex");
            syscall_set.insert(head + "SchedSetaffinity");
            syscall_set.insert(head + "SchedGetaffinity");
            syscall_set.insert(head + "IoSetup");
            syscall_set.insert(head + "IoDestroy");
            syscall_set.insert(head + "IoGetevents");
            syscall_set.insert(head + "IoSubmit");
            syscall_set.insert(head + "IoCancel");
            syscall_set.insert(head + "Fadvise64");
            syscall_set.insert(head + "Mbind");
            syscall_set.insert(head + "SetMempolicy");
            syscall_set.insert(head + "GetMempolicy");
            syscall_set.insert(head + "InotifyInit");
            syscall_set.insert(head + "InotifyAddWatch");
            syscall_set.insert(head + "InotifyRmWatch");
            syscall_set.insert(head + "Unshare");
            syscall_set.insert(head + "SyncFileRange");
            syscall_set.insert(head + "Signalfd");
            syscall_set.insert(head + "Fallocate");
            syscall_set.insert(head + "Signalfd4");
            syscall_set.insert(head + "RecvMMsg");
            syscall_set.insert(head + "Syncfs");
            syscall_set.insert(head + "SendMMsg");
            syscall_set.insert(head + "Mlock2");
            syscall_set.insert(head + "Pwritev2");

        }

        int getFuncCallSites(Function &F, int depth) {
            // func_sum is the sum of functions this call sites contain
            // temp_sum is the sub_func_sum collected to print '|'
            int func_sum = 0;
            int temp_sum = 0;

            // set to store the function name to avoid brother duplicate
            std::set<llvm::StringRef> func_set;

            // Here is a traversal of Basic Blocks inside a function 
            for (BasicBlock &bb : F) {
                // Here we go through each intruction to find 'call' instruction
                for (Instruction &i : bb) {
                    // use dyn_cast<CallInst> to check, maybe InstVisitor also works
                    if (CallInst *callInst = dyn_cast<CallInst>(&i)) {
                        // if it is an indirect call, here we IGNORE it!
                        if (callInst->getCalledFunction() == NULL) {
                            // we can not get this call function name
                            continue;
                        }
                        // check if have searched this function
                        if (func_set.find(callInst->getCalledFunction()->getName()) != func_set.end()) {
                            // already stored in the set, skip
                            continue;
                        }
                        // check the number of the instructions inside the function
                        if (callInst->getCalledFunction()->getInstructionCount() < MIN_INS_NUM) {
                            continue;
                        }
                        // Every time find a function call, counter plus 1
                        func_sum += 1;
                        // store in the set
                        func_set.insert(callInst->getCalledFunction()->getName());
                        // Print some format charactors
                        for (int k=0; k<depth; k++)
                            errs() << "    ";
                        // Move the cursor to print '|', in order to connect nearest brother node
                        for (int k=0; k<temp_sum; k++) {
                            MOVEUP(1);
                            errs() << "|";
                            MOVEDLEFT(1);
                        }
                        // Reset the cursor
                        MOVEDOWN(temp_sum);
                        // Reset the temp counter
                        temp_sum = 0;
                        // Output function name
                        errs() << "|---" << callInst->getCalledFunction()->getName() << "()\n";
                        // check depth
                        if(depth < MAX_DEPTH) {
                            // Recursivly calling to get further call sites
                            temp_sum = getFuncCallSites(*callInst->getCalledFunction(), depth+1);
                            // Collect subtree function sum
                            func_sum += temp_sum;
                        }
                    }
                }
            }
            return func_sum;
        }

        bool runOnFunction(Function &F) {
            // When the function name is what we want, that is a syscall function name, search it
            if(syscall_set.find(std::string(F.getName())) != syscall_set.end()) {
                errs() << "Function " << F.getName() << "()\n";
                getFuncCallSites(F, 0);
                errs() << "**********************************" << "\n\n";
            }
            return false;
        }
    };
}

char FindFunc::ID = 0;
static RegisterPass<FindFunc> X("find-func", "find function call site", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new FindFunc()); });