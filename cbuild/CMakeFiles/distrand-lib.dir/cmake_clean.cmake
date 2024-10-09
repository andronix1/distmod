file(REMOVE_RECURSE
  "libdistrand-lib.a"
  "libdistrand-lib.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/distrand-lib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
