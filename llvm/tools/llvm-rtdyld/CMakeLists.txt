set(LLVM_LINK_COMPONENTS RuntimeDyld)

add_llvm_tool(llvm-rtdyld
  llvm-rtdyld.cpp
  )
