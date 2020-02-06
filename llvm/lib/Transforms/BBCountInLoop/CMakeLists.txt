add_llvm_library( LLVMCountLoopBBPass MODULE
  Count_Blocks_In_Loops.cpp

  PLUGIN_TOOL
  opt
  )