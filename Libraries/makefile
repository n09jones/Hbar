include $(CURDIR)/../mkfl_include/mkfl_defs

sources = $(notdir $(wildcard src/$(1)/*.cpp))
prereqs = $(subst .cpp,.prq,$(sources))
objects = $(subst .cpp,.o,$(sources))

.PHONY : clean all
.INTERMEDIATE : $(foreach lib,$(LIBS),$(call objects,$(lib)))

all : $(addsuffix .a,$(addprefix lib/lib,$(LIBS)))
	$(COMPLETE_MESSAGE)

lib/lib%.a : $$(call objects,$$*)
	mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

%.prq : $$(*F).cpp include/ATRAP_base/Global_Info_Loc.h
	@set -e; ${RM} $@; mkdir -p $(@D); \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -M $< > $@.$$$$; \
	sed 's,\($(*F)\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	${RM} $@.$$$$

include/ATRAP_base/Global_Info_Loc.h :
	@set -e; ${RM} $@; \
	echo '#ifndef GLOBAL_INFO_LOC_H' > $@; \
	echo '#define GLOBAL_INFO_LOC_H' >> $@; \
	echo 'const boost::filesystem::path tmp_dir { $(TMP_DIR) };' >> $@; \
	echo 'const boost::filesystem::path large_storage_dir { $(LRG_STRG_DIR) };' >> $@; \
	echo '#endif /* GLOBAL_INFO_LOC_H */' >> $@

ifneq ($(MAKECMDGOALS),clean)
-include $(foreach lib,$(LIBS),$(addprefix prq/$(lib)/,$(call prereqs,$(lib))))
endif

clean :
	$(RM) -r lib prq include/ATRAP_base/Global_Info_Loc.h
