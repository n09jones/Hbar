(* ::Package:: *)

(* ::Title::Initialization:: *)
(*(*(*(*(*(*(*Atomic Calculations for n=1, 2 in H*)*)*)*)*)*)*)


(* ::Chapter::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Definitions*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define Bases*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*General Definitions*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Use the list assgn to select the element of basis with the same key values (in the same order).*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
MakeKet[assgn_,basis_]:=
Module[{ket,keys},
keys=Keys[basis[[1]]];
If[Length[assgn]!=Length[keys],Throw["Input should be: {"<>StringRiffle[keys,", "]<>"}"];];
ket=AssociationThread[keys->assgn];
If[!MemberQ[basis,ket],Throw["Desired ket does not correspond to a member of the desired basis"];];
Return[ket];
];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Get the index of ket in basis*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
ToIdx[ket_,basis_]:=
Module[{poslist},
poslist=Position[basis,ket];
Switch[Length[poslist],
1,Return[poslist[[1]][[1]]];,
0,Throw["Ket not in basis"],
_,Throw["Unexpected number of kets in basis: "<>ToString[Length[poslist]]];
];
];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Get the ket corresponding to idx in basis.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
ToKet[idx_,basis_]:=
Module[{},
If[(idx<1)||(idx>Length[basis]),Throw["Out of range"];];
Return[basis[[idx]]];
];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Convert function func[bra, ket], where bra and ket come from brabasis and ketbasis, respectively, into the corresponding matrix.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
FunctoMat[func_,brabasis_,ketbasis_]:=
Table[func[bra,ket],{bra,brabasis},{ket,ketbasis}];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define convenient objects and versions of the functions above with the basis pre-loaded. Makes:*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- [tag]Basis*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- [tag]Keys*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- Make[tag]Ket[assgn]*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- To[tag]Idx[ket]*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- To[tag]Ket[idx]*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- Functo[tag]Mat*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
MakeBasisPackage[tag_,basis_]:=
Module[{lhslist,rhslist},
Evaluate[ToExpression[tag<>"Basis"]]=basis;
Evaluate[ToExpression[tag<>"Keys"]]=Keys[basis[[1]]];
Evaluate[ToExpression["Make"<>tag<>"Ket"]][assgn_]:=MakeKet[assgn,basis];
Evaluate[ToExpression["To"<>tag<>"Idx"]][ket_]:=ToIdx[ket,basis];
Evaluate[ToExpression["To"<>tag<>"Ket"]][idx_]:=ToKet[idx,basis];
Evaluate[ToExpression["Functo"<>tag<>"Mat"]][func_]:=FunctoMat[func,basis,basis];
Protect/@{tag<>"Basis",tag<>"Keys","Make"<>tag<>"Ket","To"<>tag<>"Idx","To"<>tag<>"Ket","Functo"<>tag<>"Mat"};
];


(* ::Input::Initialization:: *)
Protect[MakeKet,ToIdx,ToKet,FunctoMat,MakeBasisPackage];


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Basis Definitions*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define the LSI Basis*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
MakeLSIBasis[tag_,L_]:=MakeBasisPackage[tag,Flatten[Table[<|"L"->L,"mL"->mL,"S"->1/2,"mS"->mS,"I"->1/2,"mI"->mI|>,{mL,-L,L},{mS,-1/2,1/2},{mI,-1/2,1/2}]]];
MakeLSIBasis["SLSI",0];
MakeLSIBasis["PLSI",1];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define the JI Basis*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
MakeJIBasis[tag_,L_]:=MakeBasisPackage[tag,Flatten[Table[<|"L"->L,"S"->1/2,"J"->J,"mJ"->mJ,"I"->1/2,"mI"->mI|>,{J,Abs[L-(1/2)],Abs[L+(1/2)]},{mJ,-J,J},{mI,-1/2,1/2}]]];
MakeJIBasis["SJI",0];
MakeJIBasis["PJI",1];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define the F Bases*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
MakeFBasis[tag_,L_]:=MakeBasisPackage[tag,Flatten[Table[<|"L"->L,"S"->1/2,"I"->1/2,"J"->J,"F"->F,"mF"->mF|>,{J,Abs[L-(1/2)],Abs[L+(1/2)]},{F,Abs[J-(1/2)],Abs[J+(1/2)]},{mF,-F,F}]]];
MakeFBasis["SF",0];
MakeFBasis["PF",1];


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Basis Conversions*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Suppress warnings for unphysical inputs to the function generating Clebsch-Gordon coefficients.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
Off[ClebschGordan::phy];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define functions and matrices representing < JIbra | Fket > and < Fbra | JIket >.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
JIdotF[JIbra_,Fket_]:=
If[SameQ@@KeyTake[{JIbra,Fket},{"J","L"}],
ClebschGordan[{JIbra["J"],JIbra["mJ"]},{JIbra["I"],JIbra["mI"]},{Fket["F"],Fket["mF"]}],
0];
FdotJI[Fbra_,JIket_]:=Conjugate[JIdotF[JIket,Fbra]];
SJIdotFmat=FunctoMat[JIdotF,SJIBasis,SFBasis];
SFdotJImat=FunctoMat[FdotJI,SFBasis,SJIBasis];
PJIdotFmat=FunctoMat[JIdotF,PJIBasis,PFBasis];
PFdotJImat=FunctoMat[FdotJI,PFBasis,PJIBasis];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define functions and matrices representing < LSIbra | JIket > and < JIbra | LSIket >.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
LSIdotJI[LSIbra_,JIket_]:=
If[SameQ@@KeyTake[{LSIbra,JIket},{"L","mI"}],ClebschGordan[{LSIbra["L"],LSIbra["mL"]},{LSIbra["S"],LSIbra["mS"]},{JIket["J"],JIket["mJ"]}],
0];
JIdotLSI[JIbra_,LSIket_]:=Conjugate[LSIdotJI[LSIket,JIbra]];
SLSIdotJImat=FunctoMat[LSIdotJI,SLSIBasis,SJIBasis];
SJIdotLSImat=FunctoMat[JIdotLSI,SJIBasis,SLSIBasis];
PLSIdotJImat=FunctoMat[LSIdotJI,PLSIBasis,PJIBasis];
PJIdotLSImat=FunctoMat[JIdotLSI,PJIBasis,PLSIBasis];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define a matrix and some functions to convert matrices and vectors between bases.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
FINdotINITmat[initbasis_,finbasis_]:=
<|{SLSIBasis,SJIBasis}->SJIdotLSImat,{SLSIBasis,SFBasis}->SFdotJImat.SJIdotLSImat,{SJIBasis,SLSIBasis}->SLSIdotJImat,{SJIBasis,SFBasis}->SFdotJImat,{SFBasis,SLSIBasis}->SLSIdotJImat.SJIdotFmat,{SFBasis,SJIBasis}->SJIdotFmat,{PLSIBasis,PJIBasis}->PJIdotLSImat,{PLSIBasis,PFBasis}->PFdotJImat.PJIdotLSImat,{PJIBasis,PLSIBasis}->PLSIdotJImat,{PJIBasis,PFBasis}->PFdotJImat,{PFBasis,PLSIBasis}->PLSIdotJImat.PJIdotFmat,{PFBasis,PJIBasis}->PJIdotFmat|>[{initbasis,finbasis}];
convertvec[initbasis_,finbasis_,vec_]:=vec.Transpose[(FINdotINITmat[initbasis,finbasis])];
convertmat[initbasis_,finbasis_,mat_]:=(FINdotINITmat[initbasis,finbasis]).mat.(FINdotINITmat[finbasis,initbasis]);


(* ::Input::Initialization:: *)
Protect[JIdotF,FdotJI,SJIdotFmat,PJIdotFmat,SFdotJImat,PFdotJImat,LSIdotJI,JIdotLSI,SLSIdotJImat,PLSIdotJImat,SJIdotLSImat,PJIdotLSImat,FINdotINITmat,convertvec,convertmat];


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define Constants*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Quantum and E&M Constants*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*NIST values (along with derived values) in SI units*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
h=6626070040 10^-43;
hbar=h/(2 \[Pi]);
me=910938356 10^-39;
mp=1672621898 10^-36;
\[Mu]pe=(1/mp+1/me)^-1;
\[Alpha]=72973525664 10^-13;
c=299792458;
\[Mu]0=4 \[Pi] 10^-7;
gL=1-(me/mp);
\[Mu]B=9274009994 10^-33;
gSbare=200231930436182 10^-14;
gS[n_]:=gSbare*(1-\[Alpha]^2/(3 n^2)+\[Alpha]^4/(2 n^3) (1/(2 n)-2/3)+\[Alpha]^3/(4 \[Pi] n^2)+\[Alpha]^2/(2 n^2) me/mp);
\[Mu]N=5050783699 10^-36;
gNbare=-5585694702 10^-9;
gN[n_]:=gNbare*(1-\[Alpha]^2/(3 n^2)+\[Alpha]^2/(6 n^2) me/mp ((3+4*1793/1000)/(1+1793/1000)));
qe=16021766208 10^-29;
a0=hbar/(\[Mu]pe c \[Alpha]);


(* ::Input::Initialization:: *)
Protect[h,hbar,me,mp,\[Mu]pe,\[Alpha],c,\[Mu]0,gL,\[Mu]B,gS,\[Mu]N,gN,qe,a0];


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Hydrogen Energy Levels*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*These constants give the energies of F basis states relative to {n,F,J,L,mF}={1,1,1/2,S(=0),mF}. Each state with principle number n, angular momentum L,and hyperfine number F is assigned energy EnLhF if J=1/2 and EnL3hF if J=3/2. The rational numbers below are the splittings in Hertz,so h, which is given in the previous section in units of Js, ensures that all energies are in J. The numbers here are found in "A critical compilation of experimental data on spectral lines and energy levels of hydrogen, deuterium, and tritium" by A.E Kramida.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
AbsSCent[n_]:=Switch[n,1,0,2,2466061413187035 h,_,Throw["n must be 1 or 2"]];


(* ::Input::Initialization:: *)
FSCentRelSCent[n_,Fket_]:=Module[{},
If[n==1&&Fket["L"]==0,Return[0];];
If[n==2&&Fket["L"]==0,Return[0];];
If[n==2&&Fket["L"]==1&&Fket["J"]==1/2,Return[-1057847000 h ];];
If[n==2&&Fket["L"]==1&&Fket["J"]==3/2,Return[9911283000 h ];];
Throw["Invalid parameters"];
];


(* ::Input::Initialization:: *)
HFsplitnoP[n_,Fket_]:=Module[{},
If[n==1&&Fket["L"]==0,Return[14204057517667 10^-4 h];];
If[n==2&&Fket["L"]==0,Return[1775568343 10^-1 h];];
If[n==2&&Fket["L"]==1,Return[0];];
Throw["Invalid parameters"];
];


(* ::Input::Initialization:: *)
HFsplitP[n_,Fket_]:=Module[{},
If[Fket["L"]==0,Return[0];];
If[n==2&&Fket["L"]==1&&Fket["J"]==1/2,Return[59220000 h];];
If[n==2&&Fket["L"]==1&&Fket["J"]==3/2,Return[23651570 h];];
Throw["Invalid parameters"];
];


(* ::Input::Initialization:: *)
HFoffset[HFsplit_,n_,Fket_]:=Module[{lownum,hinum},
lownum=(2*Abs[Fket["J"]-Fket["I"]])+1;
hinum=(2*Abs[Fket["J"]+Fket["I"]])+1;
Return[HFsplit*If[Fket["F"]==Abs[Fket["J"]+Fket["I"]],lownum,-hinum]/(lownum+hinum)];
];


(* ::Input::Initialization:: *)
ErelSCent[n_,Fket_]:=FSCentRelSCent[n,Fket]+HFoffset[HFsplitnoP[n,Fket],n,Fket];


(* ::Input::Initialization:: *)
AbsE[n_,Fket_]:=AbsSCent[n]+ErelSCent[n,Fket];


(* ::Input::Initialization:: *)
Protect[AbsSCent,FSCentRelSCent,HFsplitnoP,HFsplitP,HFoffset,ErelSCent,AbsE];


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define Operators*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Angular Momentum*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define spin matrices.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
spin0={{{0}},{{0}},{{0}}};
spinh=(1/2)*{{{0,1},{1,0}},{{0,-I},{I,0}}, {{1,0},{0,-1}}};
spin1={{{0,1,0},{1,0,1},{0,1,0}}/Sqrt[2],{{0,-I,0},{I,0,-I},{0,I,0}}/Sqrt[2],{{1,0,0},{0,0,0},{0,0,-1}}};


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define a function for representing spin in the LSI basis. This function returns < LSIbra | Subscript[*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*\!\(\*OverscriptBox[\(O\), \(^\)]\), comp]^\.08 | LSIket >. Spinkey is the key for finding the representations of *)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*\!\(\*OverscriptBox[\(O\), \(^\)]\) to which the bra and ket belong. Spinzkey is the key for finding the Subscript[*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*\!\(\*OverscriptBox[\(O\), \(^\)]\), z] eigenvalues for the bra and ket.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
LSIspin[LSIbra_,LSIket_,spinkey_,spinzkey_,comp_]:=
Module[{spinass,spinmat,offset},
If[spinkey==spinzkey,Throw["Spin and spin-z keys must differ"]];
If[(comp<1)||(comp>3),Throw["Comp must be in {1, 2, 3}"];];
If[!(SameQ@@KeyDrop[{LSIbra,LSIket},spinzkey]),Return[0];];
spinass=<|0->spin0,1->spinh,2->spin1|>;
spinmat=(spinass[Round[2*LSIbra[spinkey]]])[[comp]];
offset=LSIbra[spinkey]+1;
Return[spinmat[[offset-LSIbra[spinzkey],offset-LSIket[spinzkey]]]];
];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define a function and matrices representing the angular momentum operators.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
LSIL[LSIbra_,LSIket_,comp_]:=hbar*LSIspin[LSIbra,LSIket,"L","mL",comp];
SLSILmat=Table[FunctoSLSIMat[Function[{LSIbra,LSIket},LSIL[LSIbra,LSIket,comp]]],{comp,1,3}];
PLSILmat=Table[FunctoPLSIMat[Function[{LSIbra,LSIket},LSIL[LSIbra,LSIket,comp]]],{comp,1,3}];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define a function and matrices representing the electron spin angular momentum operators.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
LSIS[LSIbra_,LSIket_,comp_]:=hbar*LSIspin[LSIbra,LSIket,"S","mS",comp];
SLSISmat=Table[FunctoSLSIMat[Function[{LSIbra,LSIket},LSIS[LSIbra,LSIket,comp]]],{comp,1,3}];
PLSISmat=Table[FunctoPLSIMat[Function[{LSIbra,LSIket},LSIS[LSIbra,LSIket,comp]]],{comp,1,3}];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define a function and matrices representing the proton spin angular momentum operators.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
LSII[LSIbra_,LSIket_,comp_]:=hbar*LSIspin[LSIbra,LSIket,"I","mI",comp];
SLSIImat=Table[FunctoSLSIMat[Function[{LSIbra,LSIket},LSII[LSIbra,LSIket,comp]]],{comp,1,3}];
PLSIImat=Table[FunctoPLSIMat[Function[{LSIbra,LSIket},LSII[LSIbra,LSIket,comp]]],{comp,1,3}];


(* ::Input::Initialization:: *)
Protect[spin0,spinh,spin1,LSIspin,LSIL,SLSILmat,PLSILmat,LSIS,SLSISmat,PLSISmat,LSII,SLSIImat,PLSIImat];


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Position*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*< P-basis | r | S-basis >*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
PnSLSIr[n_,LSIbra_,LSIket_,comp_]:=
Module[{Rfuncs,r,\[Theta],\[Phi],brafunc,ketfunc,rop},
If[!MemberQ[PLSIBasis,LSIbra]||!MemberQ[SLSIBasis,LSIket],Throw["Must use correct bases"]];
Rfuncs=<|{1,0}->2 Exp[-r],{2,0}->(2-r)/(2 Sqrt[2]) Exp[-r/2],{2,1}->r/(2 Sqrt[6]) Exp[-r/2]|>;
brafunc=Rfuncs[{2,LSIbra["L"]}] SphericalHarmonicY[LSIbra["L"],LSIbra["mL"],\[Theta],\[Phi]];
ketfunc=Rfuncs[{n,LSIket["L"]}] SphericalHarmonicY[LSIket["L"],LSIket["mL"],\[Theta],\[Phi]];
rop=r {Sin[\[Theta]] Cos[\[Phi]],Sin[\[Theta]] Sin[\[Phi]],Cos[\[Theta]]}[[comp]];
Return[If[SameQ@@KeyTake[{LSIbra,LSIket},{"mS","mI"}],
a0 Integrate[Conjugate[brafunc] rop ketfunc r^2 Sin[\[Theta]],{\[Phi],0,2 \[Pi]},{\[Theta],0,\[Pi]},{r,0,\[Infinity]}],
0]];
];
P1SLSIr[LSIbra_,LSIket_,comp_]:=PnSLSIr[1,LSIbra,LSIket,comp];
P2SLSIr[LSIbra_,LSIket_,comp_]:=PnSLSIr[2,LSIbra,LSIket,comp];
P1SLSIrmats=Table[FunctoMat[Function[{LSIbra,LSIket},P1SLSIr[LSIbra,LSIket,k]],PLSIBasis,SLSIBasis],{k,1,3}];
P2SLSIrmats=Table[FunctoMat[Function[{LSIbra,LSIket},P2SLSIr[LSIbra,LSIket,k]],PLSIBasis,SLSIBasis],{k,1,3}];


(* ::Input::Initialization:: *)
Protect[PnSLSIr,P1SLSIr,P2SLSIr,P1SLSIrmats,P2SLSIrmats];


(* ::Subsection::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Hamiltonian*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Define the Hamiltonian, in the LSI-basis, for the hydrogen atom in the presence of a magnetic field.*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
H[n_,L_]:=Module[{FBasis,LSIBasis,magmommat,energy0,diamag},
If[L==0,
FBasis=SFBasis;LSIBasis=SLSIBasis,
FBasis=PFBasis;LSIBasis=PLSIBasis
];
magmommat=If[L==0,
(gS[n] \[Mu]B/hbar) SLSISmat[[3]]+(gL \[Mu]B/hbar ) SLSILmat[[3]]+(gN[n] \[Mu]N/hbar) SLSIImat[[3]],
(gS[n] \[Mu]B/hbar) PLSISmat[[3]]+(gL \[Mu]B/hbar ) PLSILmat[[3]]+(gN[n] \[Mu]N/hbar) PLSIImat[[3]]
];
diamag=If[L==0,If[n==1,1,14] (qe^2 a0^2 B^2)/(4 \[Mu]pe),0];
energy0=convertmat[FBasis,LSIBasis,DiagonalMatrix[Table[ErelSCent[n,Fket]+diamag,{Fket,FBasis}]]];
Return[energy0-B magmommat];
];


(* ::Input::Initialization:: *)
\[Delta]HPHFsplit[n_,L_]:=Module[{FBasis,LSIBasis},
If[L==0,
FBasis=SFBasis;LSIBasis=SLSIBasis,
FBasis=PFBasis;LSIBasis=PLSIBasis
];
Return[convertmat[FBasis,LSIBasis,DiagonalMatrix[Table[HFoffset[HFsplitP[n,Fket],n,Fket],{Fket,FBasis}]]]];
];


(* ::Input::Initialization:: *)
Protect[H,\[Delta]HPHFsplit];


(* ::Chapter::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Quantities to Calculate*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*References include*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- Two-photon excitation dynamics in bound two-body Coulomb systems including ac Stark shift and ionization : M. Haas, U. D. Jentschura, and C. H. Keitel*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*- Two-photon decay of hydrogenic atoms : J. H. Tung, X. M. Ye, G.J. Salamo, and F. T. Chan*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Eigensystems*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Gives a map Fket/JIket -> {Absolute Energy, Energy Relative to nS Centroid, Eigenvector in LSI Basis}*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
GetEigSysAss[n_,L_]:=Module[{idx,ass,EVecs0,len,Fvec,poslist,posidx,UnorderedEigSys,FBasis,LSIBasis},
UnorderedEigSys=Eigensystem[H[n,L]];
EVecs0=Normalize/@(UnorderedEigSys/.B->0)[[2]];
ass=<||>;
If[L==0,
FBasis=SFBasis;LSIBasis=SLSIBasis;,
FBasis=PJIBasis;LSIBasis=PLSIBasis;
];
len=Length[FBasis];
For[idx=1,idx<=len,idx++,
Fvec=convertvec[FBasis,LSIBasis,UnitVector[len,idx]];
poslist=Flatten[{Position[EVecs0,Fvec],Position[EVecs0,-Fvec]}];
If[Length[poslist]!=1,Print[{Fvec,EVecs0}];Throw["Wrong number of vectors found"];];
posidx=poslist[[1]];
ass[FBasis[[idx]]]={AbsSCent[n]+UnorderedEigSys[[1]][[posidx]],UnorderedEigSys[[1]][[posidx]],Normalize[UnorderedEigSys[[2]][[posidx]]]};
];
Return[ass];
];


(* ::Input::Initialization:: *)
ass1S=GetEigSysAss[1,0];
ass2S=GetEigSysAss[2,0];
ass2Pnosplit=GetEigSysAss[2,1];


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Calculates perturbative energy and state changes*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
\[Delta]E1[ass_,key_,mat_]:=Module[{vec},
vec=ass[key][[3]];
Return[Conjugate[vec].mat.vec];
];


(* ::Input::Initialization:: *)
\[Delta]v1[ass_,key_,mat_,assconn_,\[Gamma]func_]:=Module[{\[Delta]v,vec,E,veci,Ei,keyi},
E=ass[key][[1]];
vec=ass[key][[3]];
\[Delta]v=0*assconn[Keys[assconn][[1]]][[3]];
Do[
If[key!=keyi,
Ei=assconn[keyi][[1]];
veci=assconn[keyi][[3]];
\[Delta]v+=veci*Conjugate[veci].(mat.vec)/(E-Ei+(I hbar \[Gamma]func[keyi]/2));
];
,{keyi,Keys[assconn]}];
Return[\[Delta]v];
];


(* ::Input::Initialization:: *)
\[Delta]E2[ass_,key_,mat_,assconn_,\[Gamma]func_]:=Re[\[Delta]v1[ass,key,mat,assconn,\[Gamma]func].Conjugate[mat.ass[key][[3]]]];


(* ::Input::Initialization:: *)
ass2P=Module[{ass,\[Delta]E,\[Delta]v,outarr},
ass=<||>;
Do[
\[Delta]E=\[Delta]E1[ass2Pnosplit,key,\[Delta]HPHFsplit[2,1]];
\[Delta]v=\[Delta]v1[ass2Pnosplit,key,\[Delta]HPHFsplit[2,1],ass2Pnosplit,Function[x,0]];
outarr={ass2Pnosplit[key][[1]]+\[Delta]E,ass2Pnosplit[key][[2]]+\[Delta]E,Normalize[ass2Pnosplit[key][[3]]+\[Delta]v]};
ass[key]=outarr;
,{key,Keys[ass2Pnosplit]}];
ass
];


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Stark Shifts*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
ACStarkdivI=2 ((1.39927 10^-4)-(\[Minus]2.67827 10^-5));


(* ::Input::Initialization:: *)
DCStarkdivEsq[Fket_]:=\[Delta]E2[ass2S,Fket,-qe P2SLSIrmats[[1]],ass2P,Sp2Ptot]/h;


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Rabi Frequencies*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
Rabi1S2SdivI[Fket2S_,Fket1S_]:=2*(2 2 \[Pi]  (3.68111 10^-5)) Conjugate[ass2S[Fket2S][[3]]].ass1S[Fket1S][[3]];


(* ::Input::Initialization:: *)
RabiLyAlphdivsqrtI[JIket_,Fket_]:=Table[-(qe/hbar)*Sqrt[2 \[Mu]0 c]* Conjugate[ass2P[JIket][[3]]].P1SLSIrmats[[comp]].ass1S[Fket][[3]],{comp,1,3}]


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Decay Rates*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
IonRatedivI=2*2 \[Pi] (1.20208 10^-4);
Sp2S=8.2283;


(* ::Text::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(* Formulas from RADIATIVE DECAY OF METASTABLE HYDROGENIC ATOMS by S. KLARSFELD *)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


Q[\[Xi]_]:=(3 \[Xi])/(\[Xi]+2)+(2 (\[Xi]-1))/(3- \[Xi]) Hypergeometric2F1[5,1,4-\[Xi],(\[Xi]-1) (2-\[Xi])/(6 \[Xi])];
\[Phi][y_]:=Q[2/Sqrt[1+3 y]]+Q[2/Sqrt[4-3 y]];
Sp2SEDistr[y_]:=y (1-y) \[Phi][y]^2;


(* ::Input::Initialization:: *)
EmDistr[\[Theta]_,\[Phi]_]:={Cos[\[Theta]]^2 Cos[\[Phi]]^2+Sin[\[Phi]]^2,Cos[\[Theta]]^2 Sin[\[Phi]]^2+Cos[\[Phi]]^2,Sin[\[Theta]]^2,-Sin[\[Theta]]^2 Sin[2 \[Phi]],-Sin[2 \[Theta]] Cos[\[Phi]],-Sin[2 \[Theta]] Sin[\[Phi]]} Sin[\[Theta]];
EmDistrInt=Integrate[EmDistr[\[Theta],\[Phi]],{\[Theta],0,\[Pi]},{\[Phi],0,2 \[Pi]}];
DPMParams[DPM_]:={Norm[DPM[[1]]]^2,Norm[DPM[[2]]]^2,Norm[DPM[[3]]]^2,Re[DPM[[1]] Conjugate[DPM[[2]]]],Re[DPM[[1]] Conjugate[DPM[[3]]]],Re[DPM[[2]] Conjugate[DPM[[3]]]]};


(* ::Input::Initialization:: *)
DPMParams2P[JIket_,Fket_]:=DPMParams[Table[Conjugate[ass2P[JIket][[3]]].P1SLSIrmats[[comp]].ass1S[Fket][[3]],{comp,1,3}]];
Sp2P[JIket_,Fket_]:=(\[Alpha] (ass2P[JIket][[1]]-ass1S[Fket][[1]])^3)/(2 \[Pi] c^2 hbar^3) EmDistrInt.DPMParams2P[JIket,Fket];
Sp2Ptot[JIket_]:=Sum[Sp2P[JIket,Fket],{Fket,SFBasis}];


(* ::Input::Initialization:: *)
DPMParams2SdivE[Fket2S_,Fket1S_]:=DPMParams[Table[\[Delta]v1[ass2S,Fket2S,-qe P2SLSIrmats[[1]],ass2P,Sp2Ptot].P1SLSIrmats[[comp]].ass1S[Fket1S][[3]],{comp,1,3}]];
Sp2SStarkdivEsq[Fket2S_,Fket1S_]:=(\[Alpha] (ass2S[Fket2S][[1]]-ass1S[Fket1S][[1]])^3)/(2 \[Pi] c^2 hbar^3) EmDistrInt.DPMParams2SdivE[Fket2S,Fket1S];
Sp2SStarkdivEsqtot[Fket2S_]:=Sum[Sp2SStarkdivEsq[Fket2S,Fket1S],{Fket1S,SFBasis}];


Sp2SBr2Ph[Fket2S_,Fket1S_]:=Sp2S Norm[Conjugate[ass2S[Fket2S][[3]]].ass1S[Fket1S][[3]]]^2;
DPMParams2S2Ph={1,1,0,0,0,0};


(* ::Section::Initialization:: *)
(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*(*Grid File Definitions*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)*)


(* ::Input::Initialization:: *)
dB=0.001;
Blim=4;
pts=Round[Blim/dB];


(* ::Input::Initialization:: *)
SignedStr[n_]:=If[n>=0,"","n"]<>ToString[Abs[n]];
FketToStr[N_,Fket_]:=ToString[N]<>"S_F_"<>SignedStr[Fket["F"]]<>"_mF_"<>SignedStr[Fket["mF"]];
JIketToStr[JIket_]:="2P_2J_"<>SignedStr[2 JIket["J"]]<>"_2mJ_"<>SignedStr[2 JIket["mJ"]]<>"_2mI_"<>SignedStr[2 JIket["mI"]];
ketToStr[Nket_]:=Module[{N,ket},
N=Nket[[1]];
ket=Nket[[2]];
If[MemberQ[SFBasis,ket]&&((N==1)||(N==2)),Return[FketToStr[N,ket]];];
If[MemberQ[PJIBasis,ket]&&(N==2),Return[JIketToStr[ket]];];
Throw["N, ket pair is out of range"];
];
OutputFn[str_,NketVec_]:=StringJoin[transquantdir,str,Table["_"<>ketToStr[Nket],{Nket,NketVec}],".bin"];


PrintFuncTbl[Bfunc_,fn_]:=Module[{func,tbl,Bidx,len},
func=Compile[{{B,_Real}},Evaluate[Bfunc]];
len=Length[Flatten[{func[1.]}]];
tbl=Flatten[Table[If[Bidx>0,func[Bidx*dB],Table[0,{len}]],{Bidx,0,pts}]];
BinaryWrite[fn,PrependTo[tbl,dB],"Real64"];
Close[fn];
];
