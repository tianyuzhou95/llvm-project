add_llvm_library( LLVMCountOpPass MODULE
  Count_Opcodes.cpp

  PLUGIN_TOOL
  opt
  )