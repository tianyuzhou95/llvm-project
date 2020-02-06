#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
    struct TestPass : public FunctionPass
    {
        static char ID;
        TestPass() : FunctionPass(ID) {}
        bool runOnFunction(Function &F) override {
            errs() << "TestPass: ";
            errs().write_escaped(F.getName()) << '\n';
            return false;
        }
    };
}

char TestPass::ID = 0;
static RegisterPass<TestPass> X("testpass", "Hello Test Pass", false, false);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new TestPass()); });
