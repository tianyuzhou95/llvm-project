#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/CommandLine.h"
#include <set>
#include <stack>

using namespace llvm;

cl::opt<std::string> OutputFilename("oo", cl::desc("Specify output filename"), cl::value_desc("filename"));

namespace {
    struct FindCaller : public ModulePass {
        static char ID;
        std::string functionName;
        std::set<std::string> haveFound;
        std::stack<std::string> path;
        int tabSum;

        FindCaller() : ModulePass(ID) {
            functionName = OutputFilename.getValue();
            tabSum = 0;
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<CallGraphWrapperPass>();
            AU.setPreservesAll();
        }

        void printTAB() {
            int counter = tabSum;
            while(counter > 0) {
                outs() << "  ";
                counter--;
            }
        }

        void printPath() {
            std::stack<std::string> temp;
            while(!path.empty()) {
                outs() << path.top() << "->";
                temp.push(path.top());
                path.pop();
            }
            outs() << "TARGET\n";
            while(!temp.empty()) {
                path.push(temp.top());
                temp.pop();
            }
        }

        void getAllCallers(std::string funcName, Module &M) {
            // push the func into path
            path.push(funcName);
            // check if need to print(now func is the last func, which has no caller)
            bool printFlag = true;

            for (Function &F : M) {
                if (haveFound.find(std::string(F.getName())) != haveFound.end()) {
                    continue;
                }
                if (F.getName() == funcName) {
                    // avoid recursion
                    continue;
                }
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
                            if(FT->getName() == funcName) {
                                // find caller, so do not print path
                                printFlag = false;

                                // printTAB();
                                tabSum++;
                                // outs() << "find caller " << F.getName() << "\n";
                                haveFound.insert(std::string(F.getName()));
                                // find those have not found
                                // **WARN** If you want to get ALL Path
                                // **WARN** please delete this code !!!
                                getAllCallers(std::string(F.getName()), M);
                                // haveFound.erase(std::string(F.getName()));
                                tabSum--;
                                goto endLoop;
                            }
                        }
                    }
                }
                endLoop:
                ;
            }
            if (printFlag) {
                // no callers found, print the path
                printPath();
            }
            path.pop();
        }

        bool runOnModule(Module &M) override {
            // outs() << functionName << "\n";
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