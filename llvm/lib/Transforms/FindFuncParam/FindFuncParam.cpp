#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/CommandLine.h"
#include <set>

using namespace llvm;

cl::opt<std::string> OutputFilename("oo", cl::desc("Specify output filename"), cl::value_desc("filename"));

namespace {
    struct FindFuncParam : public ModulePass {
        static char ID;

        FindFuncParam() : ModulePass(ID) {

        }

        void getAllCallers(Module &M) {            
            for (Function &F : M) {
                if (F.getName() == "kjournald2") {
                    errs() << "Find!\n";
                    for (BasicBlock &bb : F) {
                        for (Instruction &i : bb) {
                            Function * FT;
                            CallInst *callInst;
                            if (callInst = dyn_cast<CallInst>(&i)) {
                                // if there exists nested bitcast call, strip off it
                                FT = dyn_cast<Function>(callInst->getCalledValue()->stripPointerCasts());
                            } else {
                                continue;
                            }
                            if (!FT) {
                                continue;
                            } else if (FT->getName() == "prepare_to_wait") {
                                Value *val_1 = callInst->getArgOperand(0);
                                Type *val_1_ty = val_1->getType();
                                outs() << val_1->getName() << val_1_ty->getTypeID() << "\n";
                            }
                        }
                    }
                }
            }
        }

        bool runOnModule(Module &M) override {
            getAllCallers(M);
            return false;
        }
    };
}

char FindFuncParam::ID = 0;
static RegisterPass<FindFuncParam> X("find-param", "find params of a function call", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new FindFuncParam()); });