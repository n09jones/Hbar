(* ::Package:: *)

(* ::Input::Initialization:: *)
basedir="/n/regal/gabrielse_lab/njones";
outdirnm="Pulsed_Data"


outdir=FileNameJoin[{basedir,outdirnm}];
inprefix="Pulsed_";
insuffix="_Detail_Monitor";

If[!DirectoryQ[outdir],CreateDirectory[outdir];];

trapdetass=<|"Quad"->-150,"Oct"->-150,"Straight_Oct"->-200,"Oct_3"->-150,"Straight_Oct_3"->-200,"Quad_1"->-150, "Straight_Oct_Extension"->-200, "Oct_Extension"->-150|>;

(* ::Input::Initialization:: *)
infn[det_]:=FileNameJoin[{basedir,inprefix<>trap<>insuffix,"Det_"<>If[det<0,"n",""]<>ToString[Abs[det]]<>".bin"}];

(* ::Input::Initialization:: *)
Do[

trapdir=FileNameJoin[{outdir,trap}];
If[!DirectoryQ[trapdir],CreateDirectory[trapdir];];

(* Load file *)
secs = 3601;
tmp=Partition[Partition[BinaryReadList[infn[trapdetass[trap]],"Real64"],17],secs];

sur={};

Do[
If[Round[Flatten[{traj[[1]][[{9,10,11}]],{0, 0}}]] == Round[traj[[secs]][[{9,10,11,14,15}]]],
AppendTo[sur, traj[[secs]]];
];
,{traj, tmp}];

outfn=FileNameJoin[{trapdir,trap<>"_end_ICs.bin"}];
BinaryWrite[outfn,Flatten[sur],"Real64"];
Close[outfn];

,{trap,{"Quad","Oct","Straight_Oct","Oct_3","Straight_Oct_3","Quad_1", "Straight_Oct_Extension", "Oct_Extension"}}];
