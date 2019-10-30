# Hbar

The [ATRAP Collaboration](http://gabrielse.physics.harvard.edu/gabrielse/overviews/Antihydrogen/Antihydrogen.html) aims to produce antimatter hydrogen atoms in a magnetic trap, cool them using a pulsed 121 nm laser, and perform (1S-2S) spectroscopy on the cooled sample using a 243 nm continuous laser. The complexity of the trapping field makes it necessary to simulate these processes. This project provides a flexible library for performing these simulations.


## Required Packages

The requirements for this project are minimal:
- a C++14 compliant compiler
- the latest version of [boost](https://www.boost.org/)

It's also useful to have Mathematica 11.0.1.0 or later. Mathematica is only needed for regenerating data on [atomic quantities](./Transition_Quantity_Generator/Trans_Quants/). Unless a new laser is introduced or the trapping fields change dramatically (> 4 T), this shouldn't be necessary.


## Getting started

From the top directory, run `make`. To regenerate the [atomic quantities](./Transition_Quantity_Generator/Trans_Quants/) data (assuming Mathematica is installed), run `make tq`.
