(* ::Package:: *)

(* ::Input::Initialization:: *)
basedir="/n/regal/gabrielse_lab/njones/STD_Pulsed_Flip_Cooled_Run_3_1";
outdirnm="Processed_Data";


tsiminsec=1*600;


outdir=FileNameJoin[{basedir,outdirnm}];
If[!DirectoryQ[outdir],CreateDirectory[outdir];];


(* ::Input::Initialization:: *)
fnlst[trap_]:=FileNames[FileNameJoin[{basedir,trap,"Det_*.bin"}]];


getdet[fn_]:=Read[StringToStream[StringReplace[Reverse[FileNameSplit[fn]][[1]],{"Det_"->"",".bin"->"","n"->"-"}]],Number];


dettostr[det_]:=If[det<0,"n",""]<>ToString[Abs[det]];


(* ::Input::Initialization:: *)
Do[

trap=coil<>"_"<>paths;

Print["Trap : "<>trap];
trapoutdir=FileNameJoin[{outdir,trap}];
If[!DirectoryQ[trapoutdir],CreateDirectory[trapoutdir];];

particlenum=Length[BinaryReadList[fnlst[trap][[1]],"Real64"]]/(17*(tsiminsec+1));

Do[

(* Load file *)
tmp=Partition[Partition[BinaryReadList[fn,"Real64"],17],(tsiminsec+1)];
det=getdet[fn];

Print["Detuning : "<>ToString[det]];

(* 1 minute snapshots*)
snapshotfn=FileNameJoin[{trapoutdir,"snapshot_"<>dettostr[det]<>".bin"}];
BinaryWrite[snapshotfn,Flatten[Table[tmp[[particle]][[;;;;60]],{particle,1,particlenum}]],"Real64"];
Close[snapshotfn];

meanfn=FileNameJoin[{trapoutdir,"mean_"<>dettostr[det]<>".bin"}];
BinaryWrite[meanfn,Mean[tmp],"Real64"];
Close[meanfn];

,{fn,fnlst[trap]}];

,{coil,{"Quad","Oct","Straight_Oct"}},{paths,{"x","z","3"}}];
