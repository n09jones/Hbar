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
- `TMP_DIR` gives the directory to which files in `LRG_STRG_DIR` will be copied before performing memory maps. For a cluster with `LRG_STRG_DIR` physically separated from compute nodes, setting `TMP_DIR` to local node storage can significantly reduce read times.

Once these variables have been modified to reflect your preferences and the state of your filesystem, the code in this project can be compiled by simply running `make` from the top directory. If you need to regenerate the [atomic quantities](./Transition_Quantity_Generator/Trans_Quants/) data (assuming Mathematica is installed), run `make tq`.


## Guiding Principles

The following ideas hold for each


## Structure

This project is made up of a series of programs, many of which 
