#include "llvm/Pass.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

// string to move the cursor, in order to print tree shape
#define MOVEUP(x) errs() << "\033[" << (x) << "A"
#define MOVEDOWN(x) errs() << "\033[" << (x) << "B"
#define MOVEDLEFT(x) errs() << "\033[" << (x) << "D"
#define MOVEDRIGHT(x) errs() << "\033[" << (x) << "C"

using namespace llvm;

namespace {
    struct FindFunc : public FunctionPass {
        static char ID;
        FindFunc() : FunctionPass(ID) {}

        int getFuncCallSites(Function &F, int depth) {
            // func_sum is the sum of functions this call sites contain
            // temp_sum is the sub_func_sum collected to print '|'
            int func_sum = 0;
            int temp_sum = 0;
            // Here is a traversal of Basic Blocks inside a function 
            for (BasicBlock &bb : F) {
                // Here we go through each intruction to find 'call' instruction
                for (Instruction &i : bb) {
                    // use dyn_cast<CallInst> to check, maybe InstVisitor also works
                    if (CallInst *callInst = dyn_cast<CallInst>(&i)) {
                        // Every time find a function call, counter plus 1
                        func_sum += 1;
                        // Print some format charactors
                        for(int k=0; k<depth; k++)
                            errs() << "    ";
                        // Move the cursor to print '|', in order to connect nearest brother node
                        for(int k=0; k<temp_sum; k++) {
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
                        // Recursivly calling to get further call sites
                        temp_sum = getFuncCallSites(*callInst->getCalledFunction(), depth+1);
                        // Collect subtree function sum
                        func_sum += temp_sum;
                    }
                }
            }
            return func_sum;
        }

        bool runOnFunction(Function &F) {
            // When the function name is what we want, search it
            if(F.getName() == "main") {
                errs() << "Function " << F.getName() << "()\n";
                getFuncCallSites(F, 0);
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