add_llvm_library( LLVMFindFuncCall MODULE
  FindFunc.cpp

  PLUGIN_TOOL
  opt
  )