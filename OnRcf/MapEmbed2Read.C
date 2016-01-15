#include <TSystem>			

class StMaker;			
class StChain;			
class StMuDstMaker;			
class pp12TreeMaker;			

void MapEmbed2Read(char *MudstFile, char *EventidFile,char* runid, char* fileid, char *pathid, char *particle) {
// Load libraries
//  gROOT   -> Macro("loadMuDst.C");			
  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  loadSharedLibraries();

  gSystem -> Load("StMuDSTMaker");
//  gSystem -> Load("StDbBroker");
//  gSystem -> Load("StDetectorDbMaker");
//  gSystem -> Load("StTpcDb");
//  gSystem -> Load("StDbUtilities");
//  gSystem -> Load("St_db_Maker");
//  gSystem->Load("StEmcRawMaker");
//  gSystem->Load("StEmcADCtoEMaker");
//  gSystem->Load("StEEmcUtil");
//  gSystem->Load("StEEmcDbMaker");
//  gSystem->Load("StPreEclMaker");
//  gSystem->Load("StEpcMaker");
  
  gSystem -> Load("pp12Tree.so") ;			

							
  // List of member links in the chain
  StChain*                    chain  =  new StChain ;

//  TString InputFileList = "s.list";
  StMuDstMaker*          muDstMaker  =  new StMuDstMaker(0,0,"",MudstFile,"MuDst",1) ;


//  St_db_Maker *dbMk = new St_db_Maker("db","MySQL:StarDb","$STAR/StarDb","StarDb");

  
  pp12TreeMaker* AnalysisCode  =  new pp12TreeMaker(muDstMaker) ;

  cout<<"turn off and on branch"<<endl;
  // Turn off everything but Primary tracks in order to speed up the analysis and eliminate IO
  muDstMaker -> SetStatus("*",0) ;                // Turn off all branches
  //muDstMaker -> SetStatus("*",1) ;                // Turn on all branches
  muDstMaker -> SetStatus("MuEvent",1) ;          // Turn on the Event data (esp. Event number)
  muDstMaker -> SetStatus("PrimaryTracks",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("GlobalTracks",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("PrimaryVertices",1) ;    // Turn on the primary vertex data
//  muDstMaker -> SetStatus("Emc*",1);
//  muDstMaker -> SetStatus("BTofHeader",1);        // Turn on the tof header
//  muDstMaker -> SetStatus("BTofHit",1);
//  muDstMaker -> SetStatus("BTofRawHit",1);

 

  // Miscellaneous things we need before starting the chain
  if(!(AnalysisCode -> SetInputEventListFile(EventidFile)))  { cout<<"NO INPUT.. END."<<endl; delete chain; return;	} // not input embedding event list

  TString Name = "out/pp12tree.run";
  Name.Append(runid);
  Name.Append("_");
  Name.Append(fileid);
  Name.Append("_");
  Name.Append(particle);
  Name.Append("_");
  Name.Append(pathid);
  Name.Append(".root");
  AnalysisCode -> SetOutputFileName(Name) ;  

  // Loop over the links in the chain
  cout<<"Init";
  chain -> Init() ;
  cout<<" done"<<endl;
  chain -> EventLoop(1,10000000) ;
  chain -> Finish() ;
  
  // Cleanup
  delete chain ;

}


