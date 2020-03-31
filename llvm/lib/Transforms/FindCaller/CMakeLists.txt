add_llvm_library( LLVMFindCaller MODULE
  FindCaller.cpp

  PLUGIN_TOOL
  opt
  )