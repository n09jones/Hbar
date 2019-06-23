#!/bin/bash

for (( IDX=${1}; IDX<=${2}; IDX+=1 ))
do
	./1S2S_Multiple_Det_no_Scatt_Straight_Oct_x_-5000_5000_1000 --opts_fn Submission_Scripts/1S2S_Spectra_no_Scatt/Options_Files/Spec_Det/Straight_Oct_x_LO_-5000_HI_5000_INCR_1000.txt --part_group_idx 0 --part_member_idx ${IDX}
done
