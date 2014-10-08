
  FILE(GLOB_RECURSE MD5SUM_INPUT_FILES
    /home/pez2001/libjoy-dev/libjoy-0.1/debian_package/*
  )
  
  EXECUTE_PROCESS(
    COMMAND md5sum ${MD5SUM_INPUT_FILES}
    WORKING_DIRECTORY /home/pez2001/libjoy-dev/libjoy-0.1/debian_package
    OUTPUT_VARIABLE md5sum_VAR
  #  OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE md5sum_RES
  )
  # apparently md5sums start with: usr/...
  STRING(REPLACE /home/pez2001/libjoy-dev/libjoy-0.1/debian_package/
                  "" md5sum_VAR_clean
                  ${md5sum_VAR})
  FILE(WRITE /home/pez2001/libjoy-dev/libjoy-0.1/md5sums ${md5sum_VAR_clean})
