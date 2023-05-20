add_executable (calc
  GSM.cpp
  CodeGen.cpp
  Lexer.cpp
  Parser.cpp
  Sema.cpp
  )
target_link_libraries(calc PRIVATE ${llvm_libs})
