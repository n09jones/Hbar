# Hbar

The [ATRAP Collaboration](http://gabrielse.physics.harvard.edu/gabrielse/overviews/Antihydrogen/Antihydrogen.html) aims to produce antimatter hydrogen atoms in a magnetic trap, cool them using a pulsed 121 nm laser, and perform (1S-2S) spectroscopy on the cooled sample using a 243 nm continuous laser. The complexity of the trapping field makes it necessary to simulate these processes. This project provides a flexible library for performing these simulations.


## Required Packages

The requirements for this project are minimal:
- a C++14 compliant compiler
- the latest version of [Boost](https://www.boost.org/)

It's also useful to have Mathematica 11.0.1.0 or later. Mathematica is only needed for regenerating data on [atomic quantities](./Transition_Quantity_Generator/Trans_Quants/). Unless a new laser is introduced or the trapping fields change dramatically (> 4 T), this shouldn't be necessary.


## Getting started

Before compiling anything, a few environmental variables need to be set. In [mkfl_defs](./mkfl_include/mkfl_defs), `SLURM_BOOST_DIR` and `MY_MAC_BOOST_DIR` give the paths to where the Boost library is installed on the cluster I used and on my personal laptop, respectively. Two blocks later, the presence of one of these paths is used to determine where the code is being compiled, which determines how the following variables are set.
- `BOOST_W_FLAGS` and `BOOST_LDLIBS` give the necessary Boost flags.
- `LRG_STRG_DIR` gives the directory where the results of most calculations are stored.
- `TMP_DIR` gives the directory to which files in `LRG_STRG_DIR` will be copied before performing memory maps. For a cluster with `LRG_STRG_DIR` physically separated from compute nodes, setting `TMP_DIR` to local node storage can significantly reduce read times. To prevent this behavior, set the `enable_mapped_temp_files` variable in [Global_Info.h](./Libraries/include/ATRAP_base/Global_Info.h) to `false`.

Once these variables have been modified to reflect your preferences and the state of your filesystem, the code in this project can be compiled by simply running `make` from the top directory. To make the project run on a new cluster, see the [Cluster Compatibility](https://github.com/n09jones/Hbar#cluster-compatibility) section. If you need to regenerate [atomic quantity](./Transition_Quantity_Generator/Trans_Quants/) data (assuming Mathematica is installed), run `make tq`.


## Guiding Principles

The following ideas hold for each program in this project.
- Running a program with no flags or command line arguments generates a short list of flags along with descriptions of the behaviors they cause.
- Options parameterizing the physical situation being simulated (e.g. laser power) and options describing any cluster allocations are stored in an options file. The options file's path is passed to a program using the flag `--opts_fn`.
- A program prints a template options file to `path` when run with the flag `--print_opts_fn path`. This template gives all required parameters, but no values.
- If the disk space taken by the output of a program using options file `path` can be determined ahead of time, it is given by running the program with the flag `--print_out_mem path`.
- By default, a program runs locally. The flags `--cluster` and `--cluster_single` export the program's work to a cluster. The flag `--cluster` breaks the work into many jobs running in parallel while `--cluster_single` confines the work to a single core. The implementation of this step is described in [this](https://github.com/n09jones/Hbar#cluster-compatibility) section.

By enforcing these behaviors, the programs in this project become easier to use. Users need only to understand the general intent of a program. Detailed information, such as the inputs needed and the space that will be taken, can be extracted quickly, without poring over documentation or source code. Additionally, there is little burden on the user who wants to send jobs to an HPC cluster. Options file arguments control cluster allocations, though these may be omitted if default values are acceptable, and a single flag toggles whether the program runs locally or on a cluster.

Two details need to be added to this picture. First, if the work to be done is partitioned into multiple jobs on a cluster, multiple output files are produced. The tracking of which jobs still need to be done and the reassembly of the outputs after all jobs have finished is handled automatically. In this case, a program must be run multiple times with the same inputs each time. Care must be taken that all active jobs have finished before the program is run again. Once all work is complete, "Job complete" is printed.

Second, Fix_Coil_Definitions does not have the user-friendly interface described above. This program takes [raw coil data](./Coil_Defs/Coil_Def_Data/Old_Coil_Defs/) from the company that produced ATRAP's magnet and puts it into a cleaner form which is scaled, rotated, and formatted for easier use. Fix_Coil_Definitions is called by the top-level make file and shouldn't need to be run otherwise, so giving it any kind of user interface seems unnecessary.


## Structure

The top-level directory contains several subdirectories, each of which serves a specific purpose. The function of each of these parts is summarized below.

### [Coil_Defs](./Coil_Defs/)

As explained above, this directory has one program, Fix_Coil_Definitions, which transforms raw data on ATRAP's magnet coils into a format that's easier to use.

### [Transition_Quantity_Generator](./Transition_Quantity_Generator/)

This directory contains Mathematica scripts for computing atomic quantities. The results are stored in [Trans_Quants](./Transition_Quantity_Generator/Trans_Quants/) for future access by antihydrogen simulations. The scripts can be run from the top-level directory with `make tq`.

### [Calc_Fields](./Calc_Fields/)

Three programs are contained in this directory.

1. Calculate_Fields computes the magnetic fields and field derivatives generated by a single magnet coil over a spatial grid. Usually, this grid covers the entirety of the anti-atom trapping volume. 
2. Sum_Fields adds the magnetic fields and field derivatives found by Calculate_Fields for a given set of magnet coils and currents. A uniform background field may also be included. 
3. Calculate_Derived_Quantities takes the output of Sum_Fields and determines antihydrogen accelerations and magnetic field magnitudes.

Note that computations throughout this pipeline are performed on the grid specified in the first stage (Calculate_Fields).

### [Interpolate_Fields](./Interpolate_Fields/)

Only one program, Interpolate_Fields, is found in this directory. It takes the output of any of the three programs in [Calc_Fields](./Calc_Fields/) and builds an interpolation over the grid on which the values are specified. The interpolation order can be chosen to optimize accuracy or speed and memory use.

### [Hbar_Propagation](./Hbar_Propagation/)

This directory contains several programs, each implementing a different simulation. Each simulation involves antihydrogen atoms propagating through a trap described by interpolations generated by Interpolate_Fields. The specifics of the various simulations are briefly summarized below.
- Make_COIL_ICs produces a realistic distribution of atomic starting positions and velocities in the trap generated by coil COIL.
- CW_LyAlpha simulates laser cooling with a continuous laser while Pulsed_LyAlpha simulates cooling with a pulsed laser. ATRAP currently uses a pulsed cooling laser.
- CW_1S2S simulates 1S-2S spectroscopy.
- Dump_Trap tracks atoms as the magnetic trap is shut off.

The flexibility of the tools in [Libraries](./Libraries/) makes developing new simulations relatively easy.

### [Libraries](./Libraries/)

This directory contains the building blocks for the programs described above. Code is roughly divided into [general use](./Libraries/include/ATRAP_base/), [magnetic field calculations](./Libraries/include/ATRAP_EM/), [interpolation](./Libraries/include/ATRAP_Trap_Quantities/), and [kinematics/atomic internal dynamics](./Libraries/include/ATRAP_Atomic_Physics/).


## Cluster Compatibility

It's easy to make this project work on a new cluster! To do so, write a .hpp file in the [Cluster_Implementations](./Libraries/include/ATRAP_base/Template_Impl/Cluster_Implementations/) directory. This code must include three things:
1. an object specifying options file parameters used to control cluster behavior
2. a function taking command line arguments, which submits the corresponding job to a single cluster core.
3. a function taking command line arguments and a list of integers describing jobs to be done, which submits all of the jobs specified to the cluster.

These descriptions are a bit vague, but the details can be easily filled in by looking at [SLURM_Impl.hpp](./Libraries/include/ATRAP_base/Template_Impl/Cluster_Implementations/SLURM_Impl.hpp). Finally, make sure the new .hpp file is included in [Job_Manager.cpp](./Libraries/src/ATRAP_base/Job_Manager.cpp), and compile the project!
