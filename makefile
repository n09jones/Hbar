.NOTPARALLEL :

ORDERED_MK_DIRS = Libraries Coil_Defs Calc_Fields Interpolate_Fields Interpolation_Testing Hbar_Propagation Quick_Test

task_list = $(foreach task,$(ORDERED_MK_DIRS),$(1) $(task) $(2);)

.PHONY = all clean

all :
	@$(call task_list,$(MAKE) -s -j -C,CMPLT_MSSG_ON=1)
	@echo "Assembly Complete!"

clean :
	@$(call task_list,$(MAKE) -s -j -C,clean)
	@echo "Cleaning Complete!"

tq :
	@$(MAKE) -C Transition_Quantity_Generator
	@echo "Transition Quantities Generated!"

clean_tq :
	@$(MAKE) -C Transition_Quantity_Generator clean
	@echo "Transition Quantities Cleaned!"
