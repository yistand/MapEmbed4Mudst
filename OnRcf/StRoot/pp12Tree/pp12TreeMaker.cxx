#include "pp12TreeMaker.h"

#include <iostream>
#include <fstream>

#include "StMuDSTMaker/COMMON/StMuDst.h" 
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuTriggerIdCollection.h"
#include "StEvent/StEnumerations.h"
#include "StEventTypes.h"
#include "StEvent/StPrimaryVertex.h"
#include "StEvent/StRunInfo.h"
#include "StDcaGeometry.h"


#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif

ClassImp(pp12TreeMaker)


//---------------------------------------------------------------------------	
pp12TreeMaker::pp12TreeMaker(StMuDstMaker* maker) :
StMaker("pp12TreeMaker") {
// Initialize and/or zero all public/private data members here.

	mMuDstMaker = maker;		//	Pass MuDst pointer to DstAnalysisMaker Class member functions
	mMuDst = NULL;			//
	pp12tree = NULL;			//	zero the pointer to tree to store
	outputfile = NULL;		//	zero the pointer to output root file
	mOutputFileName = "";		//	histogram output file name.  will be set inside the main macro
	mTotalEmbedEvent = 0;		//	Number of embedding events
	for(int i =0; i<MAXARRAYLENGTH ; i++) mEmbedEvtList[i] = -1;

	
	mRunid = 0;	
	mEventid = 0;
	mMagField = 0;
	mVx = -999;
	mVy = -999;
	mVz = -999;
	mrefMult = 0;
	mrefMultNeg = 0;
	mrefMultPos = 0;
	mNumberOfPrimaryTracks = 0;
	mNumberOfGlobalTracks = 0;
	mNtrk = 0;
	mglobalNtrk = 0;
	for(int iadc = 0; iadc<16 ; iadc++) {
		mZDCAdc[iadc] = 0;
	}
	mZDCCointcidenceRate = 0;
	for(int i = 0; i<24; i++) {
		mBBCEAdc[i] = 0;
		mBBCWAdc[i] = 0;
		mBBCETdc[i] = 0;
		mBBCWTdc[i] = 0;
	}
	mBBCCointcidenceRate = 0;

	// primary tracks in TPC and FTPC
	for(int i = 0 ; i<MAXARRAYLENGTH ; i++) {
		mFlag[i] = 9999;
		mPt[i] = 9999;
		mPx[i] = 9999;
		mPy[i] = 9999;
		mPz[i] = 9999;
		mEta[i] = 9999;
		mPhi[i] = 9999;
		mCharge[i] = 9999;
		mNHitsPos[i] = 9999;
		mNHitsFit[i] = 9999;
		mgDca[i] = 9999;
		mgDcaD[i] = 9999;
		mid[i] = 9999;
		mdEdx[i] = 9999;
		mNHitsdEdx[i] = 9999;
		mNSigmaPion[i] = 9999;
		mNSigmaKaon[i] = 9999;
		mNSigmaProton[i] = 9999;
	}

	// global tracks in TPC and FTPC
	for(int i = 0 ; i<MAXARRAYLENGTH ; i++) {
		mglobalFlag[i] = 9999;
		mglobalPt[i] = 9999;
		mglobalPx[i] = 9999;
		mglobalPy[i] = 9999;
		mglobalPz[i] = 9999;
		mglobalEta[i] = 9999;
		mglobalPhi[i] = 9999;
		mglobalCharge[i] = 9999;
		mglobalNHitsPos[i] = 9999;
		mglobalNHitsFit[i] = 9999;
		mglobalDca[i] = 9999;
		mglobalDcaD[i] = 9999;
		mglobalid[i] = 9999;
		mglobaldEdx[i] = 9999;
		mglobalNHitsdEdx[i] = 9999;
		mglobalNSigmaPion[i] = 9999;
		mglobalNSigmaKaon[i] = 9999;
		mglobalNSigmaProton[i] = 9999;
	}
	mEventsProcessed = 0;
	mEmbedProcessed = 0;

}

//---------------------------------------------------------------------------
pp12TreeMaker::~pp12TreeMaker() {
// Destroy and/or zero out all public/private data members here.

}	

//---------------------------------------------------------------------------
Bool_t pp12TreeMaker::SetInputEventListFile(char *name) {

  mEventidFileName = name;

  ifstream flist;
  flist.open(mEventidFileName,ios::in); 
  if(!flist.is_open()) {
	  std::cout<<"cannot find "<<mEventidFileName<<endl;
	  return kFALSE;
  }
 
  int tmp;
  flist >> tmp;		// First line is run id. Second line is intentionally left blank

  while(flist >> mEmbedEvtList[mTotalEmbedEvent]) {
	  mTotalEmbedEvent++;
  }

  return kTRUE;

}

//---------------------------------------------------------------------------
Int_t pp12TreeMaker::Init() {
// Do once at the start of the analysis 
	
// Create output root file
//	cout<<"Init"<<endl;

	outputfile = new TFile( mOutputFileName, "recreate") ; // Name was set previously in calling macro

// Init tree
	pp12tree = new TTree("pp12tree","RCF data Tree 2012 pp");

	if( !InitTreeBranch(pp12tree) ) cout << "Init pp12tree not successful!" << endl ;

	return kStOK;

}


//----------------------------------------------------------------------------
Int_t pp12TreeMaker::Make() {
// Do each event

	if(mEmbedProcessed==mTotalEmbedEvent) { return kStEOF;}		// The embedding list event has all been processed

// Get 'event' data

//	cout<<"Make"<<endl;
	mMuDst = mMuDstMaker->muDst();
	StMuEvent *muEvent = mMuDst->event();


// Event cuts

	if( !mMuDst ) return kStOK;
	if( !IsMatchedEvent(muEvent) ) {return kStOK;}

// record event info. into tree

	RecordEvent(muEvent);	
/*
// Get primary 'track' data. make cuts on tracks, fill track info. into tree

	TObjArray* tracks = mMuDst->primaryTracks();		//	create a TObject array containing the primary tracks
	TObjArrayIter GetTracks(tracks);		// create an iterator to step through the tracks
	StMuTrack* track;		// Pointer to a track

	mNtrk = 0;	// = mNPos + mNNeg

	int intrk = 0;		// used to record i-th track
	while ( ( track = (StMuTrack*) GetTracks.Next() ) )  {			// Main loop for Iterating over tracks
		if( IsGoodPrimaryTrack(track) ) {
			RecordPrimaryTrack(track,intrk);
			intrk++;
		}
	}

	mNtrk = intrk;



// Get global 'track' data. make cuts on tracks, fill track info. into tree

	TObjArray* gtracks = mMuDst->globalTracks();		//	create a TObject array containing the global tracks
	TObjArrayIter GetgTracks(gtracks);		// create an iterator to step through the tracks
	StMuTrack* gtrack;		// Pointer to a track

	mglobalNtrk = 0;	

	int ingtrk = 0;		// used to record i-th global track
	while ( ( gtrack = (StMuTrack*) GetgTracks.Next() ) )  {			// Main loop for Iterating over tracks
		if( IsGoodGlobalTrack(gtrack) ) {
			RecordGlobalTrack(gtrack,ingtrk);
			ingtrk++;
		}
	}

	mglobalNtrk = ingtrk;

*/
	pp12tree->Fill();

	mEventsProcessed++ ;
	return kStOK;

}


//---------------------------------------------------------------------------------------------------------------------------
Int_t pp12TreeMaker::Finish( ) {
// Do once at the end of the analysis

// Write histograms to disk, output miscellaneous other information
	
	outputfile -> cd();
	pp12tree -> Write();

	cout << "Total Events Processed in DstMaker " << mEventsProcessed << endl;

	return kStOK;

}



//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::InitTreeBranch(TTree *pp12tree) {
// set branch for tree	


	// event wise
	pp12tree->Branch("mRunid",&mRunid,"mRunid/I");
	pp12tree->Branch("mEventid",&mEventid,"mEventid/I");
//	pp12tree->Branch("mMagField",&mMagField,"mMagField/F");
//	pp12tree->Branch("mVx",&mVx,"mVx/F");
//	pp12tree->Branch("mVy",&mVy,"mVy/F");
//	pp12tree->Branch("mVz",&mVz,"mVz/F");
//	pp12tree->Branch("mrefMult",&mrefMult,"mrefMult/s");
//	pp12tree->Branch("mrefMultNeg",&mrefMultNeg,"mrefMultNeg/s");
//	pp12tree->Branch("mrefMultPos",&mrefMultPos,"mrefMultPos/s");
	pp12tree->Branch("mNumberOfPrimaryTracks",&mNumberOfPrimaryTracks,"mNumberOfPrimaryTracks/i");
	pp12tree->Branch("mNumberOfGlobalTracks",&mNumberOfGlobalTracks,"mNumberOfGlobalTracks/i");
//	pp12tree->Branch("mNtrk",&mNtrk,"mNtrk/I");
//	pp12tree->Branch("mglobalNtrk",&mglobalNtrk,"mglobalNtrk/I");
	pp12tree->Branch("mZDCAdc",&mZDCAdc,"mZDCAdc[16]/F");
	pp12tree->Branch("mZDCCointcidenceRate",&mZDCCointcidenceRate,"mZDCCointcidenceRate/F");
	pp12tree->Branch("mBBCEAdc",&mBBCEAdc,"mBBCEAdc[24]/s");
	pp12tree->Branch("mBBCWAdc",&mBBCWAdc,"mBBCWAdc[24]/s");
	pp12tree->Branch("mBBCETdc",&mBBCETdc,"mBBCETdc[24]/s");
	pp12tree->Branch("mBBCWTdc",&mBBCWTdc,"mBBCWTdc[24]/s");
	pp12tree->Branch("mBBCCointcidenceRate",&mBBCCointcidenceRate,"mBBCCointcidenceRate/F");

/*	
	// primary track wise
	pp12tree->Branch("mFlag",&mFlag,"mFlag[mNtrk]/S");
	pp12tree->Branch("mPt",&mPt,"mPt[mNtrk]/F");
	pp12tree->Branch("mPx",&mPx,"mPx[mNtrk]/F");
	pp12tree->Branch("mPy",&mPy,"mPy[mNtrk]/F");
	pp12tree->Branch("mPz",&mPz,"mPz[mNtrk]/F");
	pp12tree->Branch("mEta",&mEta,"mEta[mNtrk]/F");
	pp12tree->Branch("mPhi",&mPhi,"mPhi[mNtrk]/F");
	pp12tree->Branch("mCharge",&mCharge,"mCharge[mNtrk]/S");
	pp12tree->Branch("mNHitsPos",&mNHitsPos,"mNHitsPos[mNtrk]/s");
	pp12tree->Branch("mNHitsFit",&mNHitsFit,"mNHitsFit[mNtrk]/s");
	pp12tree->Branch("mgDca",&mgDca,"mgDca[mNtrk]/F");
	pp12tree->Branch("mgDcaD",&mgDcaD,"mgDcaD[mNtrk]/F");
	pp12tree->Branch("mid",&mid,"mid[mNtrk]/S");
	pp12tree->Branch("mdEdx",&mdEdx,"mdEdx[mNtrk]/D");
	pp12tree->Branch("mNHitsdEdx",&mNHitsdEdx,"mNHitsdEdx[mNtrk]/s");
	pp12tree->Branch("mNSigmaPion",&mNSigmaPion,"mNSigmaPion[mNtrk]/D");
	pp12tree->Branch("mNSigmaKaon",&mNSigmaKaon,"mNSigmaKaon[mNtrk]/D");
	pp12tree->Branch("mNSigmaProton",&mNSigmaProton,"mNSigmaProton[mNtrk]/D");


	
	// global track wise
	pp12tree->Branch("mglobalFlag",&mglobalFlag,"mglobalFlag[mglobalNtrk]/S");
	pp12tree->Branch("mglobalPt",&mglobalPt,"mglobalPt[mglobalNtrk]/F");
	pp12tree->Branch("mglobalPx",&mglobalPx,"mglobalPx[mglobalNtrk]/F");
	pp12tree->Branch("mglobalPy",&mglobalPy,"mglobalPy[mglobalNtrk]/F");
	pp12tree->Branch("mglobalPz",&mglobalPz,"mglobalPz[mglobalNtrk]/F");
	pp12tree->Branch("mglobalEta",&mglobalEta,"mglobalEta[mglobalNtrk]/F");
	pp12tree->Branch("mglobalPhi",&mglobalPhi,"mglobalPhi[mglobalNtrk]/F");
	pp12tree->Branch("mglobalCharge",&mglobalCharge,"mglobalCharge[mglobalNtrk]/S");
	pp12tree->Branch("mglobalNHitsPos",&mglobalNHitsPos,"mglobalNHitsPos[mglobalNtrk]/s");
	pp12tree->Branch("mglobalNHitsFit",&mglobalNHitsFit,"mglobalNHitsFit[mglobalNtrk]/s");
	pp12tree->Branch("mglobalDca",&mglobalDca,"mglobalDca[mglobalNtrk]/F");
	pp12tree->Branch("mglobalDcaD",&mglobalDcaD,"mglobalDcaD[mglobalNtrk]/F");
	pp12tree->Branch("mglobalid",&mglobalid,"mglobalid[mglobalNtrk]/S");
	pp12tree->Branch("mglobaldEdx",&mglobaldEdx,"mglobaldEdx[mglobalNtrk]/D");
	pp12tree->Branch("mglobalNHitsdEdx",&mglobalNHitsdEdx,"mglobalNHitsdEdx[mglobalNtrk]/s");
	pp12tree->Branch("mglobalNSigmaPion",&mglobalNSigmaPion,"mglobalNSigmaPion[mglobalNtrk]/D");
	pp12tree->Branch("mglobalNSigmaKaon",&mglobalNSigmaKaon,"mglobalNSigmaKaon[mglobalNtrk]/D");
	pp12tree->Branch("mglobalNSigmaProton",&mglobalNSigmaProton,"mglobalNSigmaProton[mglobalNtrk]/D");
*/
	
	return kTRUE;
	
}


//----------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::IsMatchedEvent(StMuEvent *muEvent) {
// a loose event cuts	

	// pointer
	if(!muEvent) {
		cout << " pp12TreeMaker::IsGoodEvent No StMuEvent ! " << endl;
		return kFALSE;
	}

	mEventid = muEvent->eventNumber();

	if(mEventid==mEmbedEvtList[mEmbedProcessed]) {
		mEmbedProcessed++;
		return kTRUE;
	}

	return kFALSE;

}

/*
//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::IsGoodPrimaryTrack(StMuTrack *track) {		//	test primary track with loose cuts
	if(!track) return kFALSE;
	if(track->type()!=primary) { cout << " not primary track" << endl; return kFALSE; }
	if(track->flag()<=0 || track->flag()>1000) return kFALSE;

	double eta = track->eta();
	double pt = track->pt();
	double dcaMag = track->dcaGlobal().mag();
//	unsigned short nHitsFit = track->nHitsFit();		// this is the total nhitsfit
//	unsigned short nHitsPoss = track->nHitsPoss();



	if( !(fabs(eta) <= 1.2 || (fabs(eta) <= 4 && fabs(eta) >= 2.5)) ) return kFALSE;
	if( pt < 0 ) return kFALSE;
	if( fabs(dcaMag) > 3 ) return kFALSE;
//	if( nHitsFit < 15 ) return kFALSE;
//	if( 1.0*nHitsFit/nHitsPoss < 0.51 ) return kFALSE;

	return kTRUE;

}





//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::IsGoodGlobalTrack(StMuTrack *track) {		//	test global track with loose cuts
	if(!track) return kFALSE;
	if(track->type()!=global) { cout << " not global track" << endl; return kFALSE; }
	if(track->flag()<=0 || track->flag()>1000) return kFALSE;

	double eta = track->eta();
	double pt = track->pt();
	double dcaMag = track->dca().mag();			// dca
//	unsigned short nHitsFit = track->nHitsFit();		// this is the total nhitsfit
//	unsigned short nHitsPoss = track->nHitsPoss();



	if( !(fabs(eta) <= 1.2 || (fabs(eta) <= 4 && fabs(eta) >= 2.5)) ) return kFALSE;
	if( pt < 0 ) return kFALSE;
//	if( fabs(dcaMag) > 3 ) return kFALSE;
//	if( nHitsFit < 15 ) return kFALSE;
//	if( 1.0*nHitsFit/nHitsPoss < 0.51 ) return kFALSE;

	return kTRUE;

}


*/

//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::RecordEvent(StMuEvent *muEvent) {

	mRunid = muEvent->runNumber();
	mEventid = muEvent->eventNumber();
	mMagField = muEvent->magneticField();
	mVx = muEvent->primaryVertexPosition().x();
	mVy = muEvent->primaryVertexPosition().y();
	mVz = muEvent->primaryVertexPosition().z();


	mrefMult = muEvent->refMult();
	mrefMultNeg = muEvent->refMultNeg();
	mrefMultPos = muEvent->refMultPos();

	
	//Global vs Primary Tracks
	mNumberOfPrimaryTracks = mMuDst->numberOfPrimaryTracks();
	mNumberOfGlobalTracks = mMuDst->numberOfGlobalTracks();


	//ZDC
	StZdcTriggerDetector &ZDC = muEvent->zdcTriggerDetector();
	for(int iadc = 0; iadc<16 ; iadc++) {
		mZDCAdc[iadc] = ZDC.adc(iadc);
	}
	// zdc(0) is zdcUnAttenuated(west), zdc(4) is zdcUnAttenuated(east)
	// zdc(10) is zdcAttenuated(west), zdc(13) is zdcAttenuated(east) 
	mZDCCointcidenceRate = muEvent->runInfo().zdcCoincidenceRate();
	

	//BBC
	StBbcTriggerDetector bbc = muEvent->bbcTriggerDetector();
	for(UInt_t i = 0 ; i<bbc.numberOfPMTs(); i++) {
		const UInt_t eastWest = (i<24) ? 0 : 1 ;	// East : 0-23, West 24-47
		const UInt_t pmtId = i%24;			// pmtId: 0-23

		if( eastWest  == 0 )	{
			mBBCEAdc[pmtId] = bbc.adc(i);
			mBBCETdc[pmtId] = bbc.tdc(i);
		}
		else {
			mBBCWAdc[pmtId] = bbc.adc(i);
			mBBCWTdc[pmtId] = bbc.tdc(i);
		}
	}
	mBBCCointcidenceRate = muEvent->runInfo().bbcCoincidenceRate();


	return kTRUE;



}


//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::RecordPrimaryTrack(StMuTrack *track, Int_t itrk) {

	mFlag[itrk] = track->flag();
	mPt[itrk] = track->pt();
	mPx[itrk] = track->p().x();
	mPy[itrk] = track->p().y();
	mPz[itrk] = track->p().z();
	mEta[itrk] = track->eta();
	mPhi[itrk] = track->phi();
	mCharge[itrk] = track->charge();
//	mNHitsPos[itrk] = track->nHitsPoss(kTpcId);			// not true for FTPC tracks		2013.10.12 LY
//	mNHitsFit[itrk] = track->nHitsFit(kTpcId);			// not true for FTPC tracks		2013.10.12 LY
	mNHitsPos[itrk] = track->nHitsPoss();				// 2013.10.12 LY
	mNHitsFit[itrk] = track->nHitsFit();				// 2013.10.12 LY
	mgDca[itrk] = track->dcaGlobal().mag();
	mgDcaD[itrk] = track->dcaD();			// Signed radial component of global DCA (projected). 
	mid[itrk] = track->id();					// 2014.06.06 LY

	mdEdx[itrk] = track->dEdx();					// 2014.02.22 LY record dEdx info
	mNHitsdEdx[itrk] = track->nHitsDedx();					// 2014.02.22 LY record dEdx info
	mNSigmaPion[itrk] = track->nSigmaPion();					// 2014.02.22 LY record dEdx info
	mNSigmaKaon[itrk] = track->nSigmaKaon();					// 2014.02.22 LY record dEdx info
	mNSigmaProton[itrk] = track->nSigmaProton();					// 2014.02.22 LY record dEdx info


	return kTRUE;

}



//---------------------------------------------------------------------------------------------------------------------------
Bool_t pp12TreeMaker::RecordGlobalTrack(StMuTrack *track, Int_t itrk) {

	mglobalFlag[itrk] = track->flag();
	mglobalPt[itrk] = track->pt();
	mglobalPx[itrk] = track->p().x();
	mglobalPy[itrk] = track->p().y();
	mglobalPz[itrk] = track->p().z();
	mglobalEta[itrk] = track->eta();
	mglobalPhi[itrk] = track->phi();
	mglobalCharge[itrk] = track->charge();
//	mglobalNHitsPos[itrk] = track->nHitsPoss(kTpcId);			// not true for FTPC tracks		2013.10.12 LY
//	mglobalNHitsFit[itrk] = track->nHitsFit(kTpcId);			// not true for FTPC tracks		2013.10.12 LY
	mglobalNHitsPos[itrk] = track->nHitsPoss();				// 2013.10.12 LY
	mglobalNHitsFit[itrk] = track->nHitsFit();				// 2013.10.12 LY
	mglobalDca[itrk] = track->dca().mag();
	mglobalDcaD[itrk] = track->dcaD();
	mglobalid[itrk] = track->id();					// 2014.06.06 LY
	mglobaldEdx[itrk] = track->dEdx();					// 2014.02.22 LY record dEdx info
	mglobalNHitsdEdx[itrk] = track->nHitsDedx();					// 2014.02.22 LY record dEdx info
	mglobalNSigmaPion[itrk] = track->nSigmaPion();					// 2014.02.22 LY record dEdx info
	mglobalNSigmaKaon[itrk] = track->nSigmaKaon();					// 2014.02.22 LY record dEdx info
	mglobalNSigmaProton[itrk] = track->nSigmaProton();					// 2014.02.22 LY record dEdx info


	return kTRUE;

}




