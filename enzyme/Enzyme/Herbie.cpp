#include <llvm/Config/llvm-config.h>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include <llvm/ADT/StringRef.h>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"

#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/Program.h>

#include "llvm/Pass.h"

#include "llvm/Transforms/Utils.h"

#include <fstream>
#include <map>
#include <string>

#include "Herbie.h"
#include "Utils.h"

using namespace llvm;
#ifdef DEBUG_TYPE
#undef DEBUG_TYPE
#endif
#define DEBUG_TYPE "fp-opt"

void runViaHerbie(const std::string &cmd) {
  std::string tmpin = "/tmp/herbie_input";
  std::string tmpout = "/tmp/herbie_output";

  std::ofstream input(tmpin);
  if (!input) {
    llvm::errs() << "Failed to open input file.\n";
    return;
  }
  input << cmd;
  input.close();

  std::string Program = HERBIE_BINARY;

  llvm::StringRef Args[] = {"", "shell"};
  llvm::ArrayRef<llvm::Optional<llvm::StringRef>> Redirects = {
      llvm::StringRef(tmpin),  // stdin
      llvm::StringRef(tmpout), // stdout
      llvm::StringRef(tmpout)  // stderr
  };

  std::string ErrMsg;
  bool ExecutionFailed = false;

  llvm::errs() << "Executing: " << Program << "\n";

  llvm::sys::ExecuteAndWait(Program, Args, /*Env=*/llvm::None,
                            /*Redirects=*/Redirects,
                            /*SecondsToWait=*/0, /*MemoryLimit=*/0, &ErrMsg,
                            &ExecutionFailed);

  if (ExecutionFailed) {
    llvm::errs() << "Execution failed: " << ErrMsg << "\n";
    return;
  }

  std::ifstream output(tmpout);
  if (!output) {
    llvm::errs() << "Failed to open output file.\n";
    return;
  }

  std::string line;
  while (std::getline(output, line)) {
    llvm::errs() << line << "\n";
  }
  output.close();
}

// Run (our choice of) floating point optimizations on function `F`.
// Return whether or not we change the function.
bool fpOptimize(llvm::Function &F) {
  bool changed = false;
  // 1) Identify subgraphs of the computation which can be entirely represented
  // in herbie-style arithmetic

  llvm::errs() << "Optimizing function " << F.getName().str() << "\n";

  // 2) Make the herbie FP-style expression by
  // converting  llvm instructions into herbie string (FPNode ....)

  // 3) run fancy opts

  // runViaHerbie()

  // 4) parse the output string solution from herbieland

  // 5) convert into a solution in llvm vals/instructions
  return changed;
}

namespace {

class FPOpt final : public FunctionPass {
public:
  static char ID;
  FPOpt() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {}
  bool runOnFunction(Function &F) override { return fpOptimize(F); }
};

} // namespace

char FPOpt::ID = 0;

static RegisterPass<FPOpt> X("fp-opt",
                             "Run Enzyme/Herbie Floating point optimizations");

FunctionPass *createFPOptPass() { return new FPOpt(); }

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

#include "llvm/IR/LegacyPassManager.h"

extern "C" void AddFPOptPass(LLVMPassManagerRef PM) {
  unwrap(PM)->add(createFPOptPass());
}

FPOptNewPM::Result FPOptNewPM::run(llvm::Module &M,
                                   llvm::ModuleAnalysisManager &MAM) {
  bool changed = false;
  for (auto &F : M)
    changed |= fpOptimize(F);
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
llvm::AnalysisKey FPOptNewPM::Key;
