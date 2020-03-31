#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <set>

using namespace llvm;

namespace {
    struct FindCaller : public ModulePass {
        static char ID;
        std::string functionName;

        FindCaller() : ModulePass(ID) {
            functionName = "start_this_handle";
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<CallGraphWrapperPass>();
            AU.setPreservesAll();
        }

        void getAllCallers(std::string funcName, Module &M) {
            for (Function &F : M) {
                // if (std::string(F.getName()) == functionName) {
                //     outs() << "Find " << functionName << "\n";
                //     CallGraph & CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();
                //     CallGraphNode * CGN = CG[&F];
                //     for (CallGraphNode::iterator ti = CGN->begin(); ti != CGN->end(); ++ti) {
                //         CallGraphNode * CalleeNode = ti->second;
                //         if (CalleeNode != CG.getCallsExternalNode()) {
                //             outs() << CalleeNode->getFunction()->getName() << "\n";
                //         }
                //     }
                // }
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
                            continue;
                        } else {
                            if(FT->getName() == functionName) {
                                outs() << "find caller " << F.getName() << "\n";
                                goto endLoop;
                            }
                        }
                    }
                }
                endLoop:
                ;
            }
        }

        bool runOnModule(Module &M) override {
            getAllCallers(functionName, M);
            return false;
        }
    };
}

char FindCaller::ID = 0;
static RegisterPass<FindCaller> X("find-caller", "find callers of a function", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new FindCaller()); });