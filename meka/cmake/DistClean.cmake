
  ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
  SET(DISTCLEANED cmake.depends cmake.check_depends CMakeCache.txt cmake.check_cache *.cmake Makefile core core.* gmon.out *~)
  
  ADD_CUSTOM_COMMAND(DEPENDS clean COMMENT "distribution clean" COMMAND rm ARGS    -Rf CMakeTmp ${DISTCLEANED} TARGET  distclean)
