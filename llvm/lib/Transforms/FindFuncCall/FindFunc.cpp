#define DEBUG_TYPE "opCounter"
#include "llvm/Pass.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <map>

using namespace llvm;

namespace {
    struct FindFunc : public FunctionPass {
        static char ID;
        FindFunc() : FunctionPass(ID) {}
        bool runOnFunction(Function &F) {
            errs() << "Function " << F.getName() << "\n";
            for (BasicBlock &bb : F) {
                for (Instruction &i : bb) {
                    if (CallInst *callInst = dyn_cast<CallInst>(&i)) {
                        errs() << callInst->getCalledFunction()->getName() << "\n";
                    }
                }
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