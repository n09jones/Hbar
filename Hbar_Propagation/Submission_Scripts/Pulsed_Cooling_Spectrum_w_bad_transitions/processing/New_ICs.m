(* ::Package:: *)

(* ::Input::Initialization:: *)
basedir="/n/regal/gabrielse_lab/njones/STD_Pulsed_Cooling_Spectrum_w_bad_transitions";
outdir="/n/regal/gabrielse_lab/njones/STD_ICs";

tsiminsec=60*60;

(* ::Input::Initialization:: *)
dettostr[det_]:=If[det<0,"n",""]<>ToString[Abs[det]];
fnlst[trap_,det_]:=FileNameJoin[{basedir,trap,"Det_"<>dettostr[det]<>".bin"}];


(* ::Input::Initialization:: *)
Do[

trap=coil<>"_"<>paths;
det = <|"Quad"->-150,"Oct"->-200,"Straight_Oct"->-200|>[coil];

Print["Trap : "<>trap];

particlenum=Length[BinaryReadList[fnlst[trap, det],"Real64"]]/(17*(tsiminsec+1));

(* Load file *)
tmp=Partition[Partition[BinaryReadList[fnlst[trap, det],"Real64"],17],(tsiminsec+1)];

survidxs={};
For[idx=1,idx<=particlenum,idx++,
If[Total[Reverse[tmp[[idx]]][[1]][[14;;15]]]<0.5&&(tmp[[idx]][[1]][[9;;11]]==Reverse[tmp[[idx]]][[1]][[9;;11]]),AppendTo[survidxs,idx]];
];

meansurvfn=FileNameJoin[{outdir,trap<>"_cooled.bin"}];
BinaryWrite[meansurvfn, Flatten[Table[tmp[[idx]][[tsiminsec+1]], {idx, survidxs}]], "Real64"];
Close[meansurvfn];

,{coil,{"Quad","Oct","Straight_Oct"}},{paths,{"x","z","3"}}];
