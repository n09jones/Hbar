(* ::Package:: *)

(* ::Input::Initialization:: *)
basedir="/n/regal/gabrielse_lab/njones";
outdirnm="Pulsed_Data";


outdir=FileNameJoin[{basedir,outdirnm}];
inprefix="Pulsed_";
insuffix="_Detail_Monitor";

If[!DirectoryQ[outdir],CreateDirectory[outdir];];


(* ::Input::Initialization:: *)
fnlst[trap_]:=FileNames[FileNameJoin[{basedir,inprefix<>trap<>insuffix,"Det_*.bin"}]];


getdet[fn_]:=Read[StringToStream[StringReplace[Reverse[FileNameSplit[fn]][[1]],{"Det_"->"","_mean.bin"->"","n"->"-"}]],Number];

(* ::Input::Initialization:: *)
Do[
Print["Trap : "<>trap];
trapdir=FileNameJoin[{outdir,trap}];
If[!DirectoryQ[trapdir],CreateDirectory[trapdir];];

particlenum=Length[BinaryReadList[fnlst[trap][[1]],"Real64"]]/(17*3601);

survivingparticles=Table[k,{k,1,particlenum}];

Do[

(* Load file *)
tmp=Partition[Partition[BinaryReadList[fn,"Real64"],17],3601];
det=getdet[fn];

Print["Detuning : "<>ToString[det]];

(* Get dEvsE data *)
dEvsEfn=FileNameJoin[{trapdir,"dEvsE_"<>ToString[det]<>".bin"}];

tmp2 = Transpose[tmp, {3,1,2}];
tmp2diff = Transpose[Flatten[Transpose[tmp2[[2;;3601]] - tmp2[[1;;3600]], {2, 3, 1}], 1]];
posscat = Flatten[Position[tmp2diff[[12]]+tmp2diff[[13]], x_ /;x > 0.5]];
BinaryWrite[dEvsEfn, Flatten[Transpose[{Transpose[Flatten[Transpose[tmp2[[1;;3600]], {2, 3, 1}], 1]][[8]], tmp2diff[[8]], tmp2diff[[12]], tmp2diff[[13]]}][[posscat]]], "Real64"];
Close[dEvsEfn];

(* 1 minute snapshots*)
snapshotfn=FileNameJoin[{trapdir,"snapshot_"<>ToString[det]<>".bin"}];
BinaryWrite[snapshotfn,Flatten[Table[tmp[[particle]][[;;;;60]],{particle,1,particlenum}]],"Real64"];
Close[snapshotfn];

meanfn=FileNameJoin[{trapdir,"mean_"<>ToString[det]<>".bin"}];
BinaryWrite[meanfn,Mean[tmp],"Real64"];
Close[meanfn];

survidxs={};
For[idx=1,idx<=particlenum,idx++,
If[Total[Reverse[tmp[[idx]]][[1]][[14;;15]]]<0.5&&(tmp[[idx]][[1]][[9;;11]]==Reverse[tmp[[idx]]][[1]][[9;;11]]),AppendTo[survidxs,idx]];
];
survivingparticles=Intersection[survivingparticles,survidxs];

meansurvfn=FileNameJoin[{trapdir,"mean_surv_loc_"<>ToString[det]<>".bin"}];
BinaryWrite[meansurvfn,Mean[tmp[[survidxs]]],"Real64"];
Close[meansurvfn];

,{fn,fnlst[trap]}];


Do[

(* Load file *)
tmp=Partition[Partition[BinaryReadList[fn,"Real64"],17],3601];
det=getdet[fn];

meansurvglfn=FileNameJoin[{trapdir,"mean_surv_gl_"<>ToString[det]<>".bin"}];
BinaryWrite[meansurvglfn,Mean[tmp[[survivingparticles]]],"Real64"];
Close[meansurvglfn];

,{fn,fnlst[trap]}];

,{trap,{"Quad","Oct","Straight_Oct","Oct_3","Straight_Oct_3","Quad_1","Oct_Extension","Straight_Oct_Extension"}}];
