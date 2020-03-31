#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <set>

using namespace llvm;

namespace {
    struct DoubleFetch : public FunctionPass {
        static char ID;

        DoubleFetch() : FunctionPass(ID) {}

        bool isCopyCall(std::string name) {
            if (name.find("Copy") != std::string::npos) {
                return true;
            }
            if (name.find("copy") != std::string::npos) {
                return true;
            }
            return false;
        }

        bool runOnFunction(Function &F) override {
            int counter = 0;
            for (BasicBlock &bb : F) {
                for (Instruction &i : bb) {
                    Function * FT;
                    if (CallInst *callInst = dyn_cast<CallInst>(&i)) {
                        // if there exists nested bitcast call, strip off it
                        FT = dyn_cast<Function>(callInst->getCalledValue()->stripPointerCasts());
                    } else if (InvokeInst *invokeInst = dyn_cast<InvokeInst>(&i)) {
                        // if there exists nested bitcast invoke, strip off it
                        FT = dyn_cast<Function>(invokeInst->getCalledValue()->stripPointerCasts());
                    } else {
                        continue;
                    }
                    if (!FT) {
                        // ********
                        // gVisor or other projects exists some NULL Function call due to build error
                        // Here we just ignore them to avoid crash
                        // ********
                        continue;
                    }
                    std::string temp_name = std::string(FT->getName());
                    if (isCopyCall(temp_name)) {
                        counter += 1;
                        if (counter > 1) {
                            errs() << F.getName() << "\n";
                            return false;
                        }
                    }
                }
            }
            return false;
        }
    };
}

char DoubleFetch::ID = 0;
static RegisterPass<DoubleFetch> X("double-fetch", "find if there exist double fetch in gVisor", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new DoubleFetch()); });