include $(CURDIR)/../mkfl_include/mkfl_defs

.NOTPARALLEL :
.PHONY : clean all
LDLIBS = $(BASE_LDLIBS)

COILS = STD_Quad_Coil STD_Oct_Coil

all : $(foreach coil,$(COILS),Coil_Def_Data/$(coil).bin)
	$(COMPLETE_MESSAGE)

Coil_Def_Data/%.bin : $(executables)
	./$(executables)

clean:
	$(RM) $(executables) Coil_Def_Data/*.bin