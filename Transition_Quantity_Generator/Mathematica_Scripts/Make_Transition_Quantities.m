(* ::Package:: *)

SetDirectory[If[$InputFileName=="",NotebookDirectory[],DirectoryName[$InputFileName]]];
transquantdir=ParentDirectory[]<>"/Trans_Quants/";


If[DirectoryQ[transquantdir],DeleteDirectory[transquantdir, DeleteContents->True];];
CreateDirectory[transquantdir];


<<"Transition_Definitions.m";


(* ::Input::Initialization:: *)
Do[
PrintFuncTbl[(ass1S[Fket][[2]]-(ass1S[Fket][[2]]/.B->0)),OutputFn["Gr_Energy",{{1,Fket}}]];,{Fket,SFBasis}];


Module[{func},
Do[
func=(If[MemberQ[PJIBasis,exket],ass2P,ass2S][exket][[1]]-ass1S[Fket1S][[1]])/h;
PrintFuncTbl[func,OutputFn["Transition_Freq",{{2,exket},{1,Fket1S}}]];
,{exket,Flatten[{SFBasis,PJIBasis}]},{Fket1S,SFBasis}];
];


Module[{exisP,decayfunc,brfunc,dpmfunc},
Do[

exisP=MemberQ[PJIBasis,exket];

decayfunc=If[exisP,Sp2Ptot[exket],Sp2SStarkdivEsqtot[exket]];
PrintFuncTbl[decayfunc,OutputFn["Decay_Rate_1Ph",{{2,exket}}]];

Do[

brfunc=If[exisP,Sp2P[exket,Fket1S],Sp2SStarkdivEsq[exket,Fket1S]]/decayfunc;
PrintFuncTbl[brfunc,OutputFn["Branching_1Ph",{{2,exket},{1,Fket1S}}]];

dpmfunc=If[exisP,DPMParams2P[exket,Fket1S],DPMParams2SdivE[exket,Fket1S]];
PrintFuncTbl[dpmfunc,OutputFn["DPM_1Ph",{{2,exket},{1,Fket1S}}]];

,{Fket1S,SFBasis}];

,{exket,Flatten[{SFBasis,PJIBasis}]}];
];


Do[

PrintFuncTbl[Sp2S, OutputFn["Decay_Rate_2Ph",{{2,Fket2S}}]];

Do[

PrintFuncTbl[Sp2SBr2Ph[Fket2S,Fket1S]/Sp2S,OutputFn["Branching_2Ph",{{2,Fket2S},{1,Fket1S}}]];

PrintFuncTbl[DPMParams2S2Ph,OutputFn["DPM_2Ph",{{2,Fket2S},{1,Fket1S}}]];

,{Fket1S,SFBasis}];

,{Fket2S,SFBasis}];


Do[PrintFuncTbl[IonRatedivI,OutputFn["Ionization_Rate",{{2,Fket2S}}]],{Fket2S,SFBasis}]


Module[{func},
Do[
func=If[MemberQ[PJIBasis,exket],RabiLyAlphdivsqrtI[exket,Fket1S],Rabi1S2SdivI[exket,Fket1S]];
PrintFuncTbl[Flatten[{Re[func],Im[func]}],OutputFn["Rabi_Freq",{{2,exket},{1,Fket1S}}]];
,{exket,Flatten[{SFBasis,PJIBasis}]},{Fket1S,SFBasis}];
];


Module[{exE,func},
Do[
exE=If[MemberQ[PJIBasis,exket],ass2P[exket][[2]],ass2S[exket][[2]]];
func=(exE-ass1S[Fket1S][[2]])/h;
PrintFuncTbl[func-(func/.B->0),OutputFn["Zeeman_Shift",{{2,exket},{1,Fket1S}}]];
,{exket,Flatten[{SFBasis,PJIBasis}]},{Fket1S,SFBasis}];
];


Do[
PrintFuncTbl[ACStarkdivI,OutputFn["AC_Stark_Shift",{{2,Fket2S},{1,Fket1S}}]];
PrintFuncTbl[DCStarkdivEsq[Fket2S],OutputFn["DC_Stark_Shift",{{2,Fket2S},{1,Fket1S}}]];
,{Fket2S,SFBasis},{Fket1S,SFBasis}];


(* ::Input::Initialization:: *)
Module[{tbl,fn},
tbl=Table[Sp2SEDistr[y/pts],{y,0,pts}];
tbl/=Max[tbl];
fn=OutputFn["TwoPhoton_Energy_Distribution",{}];
BinaryWrite[fn,PrependTo[tbl,1/pts],"Real64"];
Close[fn];
];
