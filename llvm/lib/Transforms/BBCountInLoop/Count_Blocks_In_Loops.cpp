#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
using namespace llvm;

namespace {
    struct BBInLoop : public FunctionPass {
        static char ID;
        BBInLoop() : FunctionPass(ID) {}

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<LoopInfoWrapperPass>();
            AU.setPreservesAll();
        }

        void countBlocksInLoop(Loop *L, unsigned nesting) {
            unsigned numBlocks = 0;
            Loop::block_iterator bb;
            for (bb = L->block_begin(); bb != L->block_end(); bb++) {
                numBlocks++;
            }
            errs() << "Loop Level: " << nesting << ": #BBs = " << numBlocks << "\n";
            std::vector<Loop*> subLoops = L->getSubLoops();
            Loop::iterator j;
            for (j = subLoops.begin(); j != subLoops.end(); j++) {
                countBlocksInLoop(*j, nesting+1);
            }
        }
        
        bool runOnFunction(Function &F) override {
            errs() << "Function " << F.getName() << "\n";

            LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
            for (LoopInfo::iterator i = LI.begin(), e = LI.end(); i != e; i++) {
                countBlocksInLoop(*i, 0);
            }
            return false;
        }
    };
}

char BBInLoop::ID = 0;
static RegisterPass<BBInLoop> X("count-loop-bb", "count loop's basic blocks", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new BBInLoop()); });