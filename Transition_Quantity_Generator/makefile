.PHONY : clean all

ifneq ("$(shell command -v wolfram)","")
MATH_CMD = "wolfram"
else ifneq ("$(shell command -v /Applications/Mathematica.app/Contents/MacOS/WolframKernel)","")
MATH_CMD = "/Applications/Mathematica.app/Contents/MacOS/WolframKernel"
else
MATH_CMD = "%ProgramFiles%\Wolfram Research\Mathematica\11.1\wolfram"
endif

all : Mathematica_Scripts/Make_Transition_Quantities.m
	@$(MATH_CMD) -script Mathematica_Scripts/Make_Transition_Quantities.m

clean:
	@$(RM) -r Trans_Quants
