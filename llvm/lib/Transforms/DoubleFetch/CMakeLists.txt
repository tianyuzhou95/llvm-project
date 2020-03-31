add_llvm_library( LLVMDoubleFetch MODULE
  DoubleFetch.cpp

  PLUGIN_TOOL
  opt
  )