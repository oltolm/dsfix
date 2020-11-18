include_guard()

macro(add_cv2pdb_target t new_exe_file pdb_file)
  if(NOT cv2pdb)
    message(FATAL_ERROR "variable \"cv2pdb\" not set")
  endif()

  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/pdb/")

  add_custom_command(
    # OUTPUT "pdb/$<TARGET_FILE_NAME:${t}>" "pdb/$<TARGET_FILE_BASE_NAME:${t}>.pdb"
    OUTPUT "pdb/${new_exe_file}" "pdb/${pdb_file}"
    COMMAND "${cv2pdb}" "$<TARGET_FILE:${t}>" "${PROJECT_BINARY_DIR}/pdb/$<TARGET_FILE_NAME:${t}>" "${PROJECT_BINARY_DIR}/pdb/$<TARGET_FILE_BASE_NAME:${t}>.pdb"
    DEPENDS ${t}
    COMMENT "creating PDB files for target ${t}")
  add_custom_target(${t}_pdb DEPENDS "pdb/${new_exe_file}" "pdb/${pdb_file}")
endmacro()
