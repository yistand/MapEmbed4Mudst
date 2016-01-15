
#ifndef pp12TreeMaker_def
#define pp12TreeMaker_def

#include "StMaker.h"
#include "TClonesArray.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "TTree.h"
#include "TString.h"


class TFile;
class TTree;
class TString;
class TClonesArray;
class StFile;
class StMuDst;
class StMuDstMaker;
class StMuEvent;
class StMuTrack;
class StMuPrimaryVertex;
class StPrimaryVertex;


#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif

#define MAXARRAYLENGTH	4000

class pp12TreeMaker : public StMaker {
	public:
		pp12TreeMaker(StMuDstMaker* maker);		//	Constructor

		virtual ~pp12TreeMaker();		//	Destructor
		Int_t Init();		//	Initiliaze the analysis tools ... done once
		Int_t Make();		//	The main analysis that is done on each event
		Int_t Finish();		//	Finish the analysis, close files, and clean up


		void SetOutputFileName(TString name) {mOutputFileName = name;}// Make name available to member functions
		Bool_t SetInputEventListFile(char *name); //Filename for embedding event list

	protected:
		Bool_t InitTreeBranch(TTree *pp12tree);		//	Initiliaze pp12tree branch, assoicate the branch with variable members
		Bool_t IsMatchedEvent(StMuEvent *muEvent);		//	check if it is the event matched in embedding data event list
		Bool_t RecordEvent(StMuEvent *muEvent);		//	record event info. in the variable member
		Bool_t RecordPrimaryTrack(StMuTrack *track, Int_t itrk);		//	record track info. in the variable member in TPC or FTPC
		Bool_t RecordGlobalTrack(StMuTrack *track, Int_t itrk);		//	record track info. in the variable member in TPC or FTPC
		
	private:
		StMuDstMaker* mMuDstMaker;		//	Make MuDst pointer available to member functions
		StMuDst* mMuDst;		//	MuDst pointer
		TFile *outputfile;		//	ouputfile pointer
		TString mOutputFileName;		//	Name of the output file
		
		TString mEventidFileName;		//	Embedding event list file

		int mTotalEmbedEvent;			//	Embedding event list 
		int mEmbedEvtList[MAXARRAYLENGTH];

		UInt_t mEventsProcessed;		// Number of Events read and processed
		Int_t mEmbedProcessed;			// Number of Events read in Embedding list

		TTree *pp12tree;			//	tree to store event and track info.

		Int_t mRunid;
		Int_t mEventid;
		Float_t mMagField;
		Float_t	mVx;
		Float_t mVy;
		Float_t mVz;
		UShort_t mrefMult;
		UShort_t mrefMultNeg;
		UShort_t mrefMultPos;
		UInt_t mNumberOfPrimaryTracks;			// StMudst::numberOfPrimaryTracks
		UInt_t mNumberOfGlobalTracks;			// StMudst::numberOfGlobalTracks;
		Int_t mNtrk;
		Int_t mglobalNtrk;
		Float_t mZDCAdc[16];
		Float_t mZDCCointcidenceRate;
		UShort_t mBBCEAdc[24];
		UShort_t mBBCWAdc[24];
		UShort_t mBBCETdc[24];
		UShort_t mBBCWTdc[24];
		Float_t mBBCCointcidenceRate;

		// primary track in TPC or FTPC
		Short_t mFlag[MAXARRAYLENGTH];
		Float_t mPt[MAXARRAYLENGTH];
		Float_t mPx[MAXARRAYLENGTH];
		Float_t mPy[MAXARRAYLENGTH];
		Float_t mPz[MAXARRAYLENGTH];
		Float_t mEta[MAXARRAYLENGTH];
		Float_t mPhi[MAXARRAYLENGTH];
		Short_t mCharge[MAXARRAYLENGTH];
		UShort_t mNHitsPos[MAXARRAYLENGTH];
		UShort_t mNHitsFit[MAXARRAYLENGTH];
		Float_t mgDca[MAXARRAYLENGTH];
		Float_t mgDcaD[MAXARRAYLENGTH];				//  Signed radial component of global DCA (projected). 
		Short_t mid[MAXARRAYLENGTH];
		Double_t mdEdx[MAXARRAYLENGTH];
		UShort_t mNHitsdEdx[MAXARRAYLENGTH];
		Double_t mNSigmaPion[MAXARRAYLENGTH];
		Double_t mNSigmaKaon[MAXARRAYLENGTH];
		Double_t mNSigmaProton[MAXARRAYLENGTH];



		// global track in TPC or FTPC 
		Short_t mglobalFlag[MAXARRAYLENGTH];
		Float_t mglobalPt[MAXARRAYLENGTH];
		Float_t mglobalPx[MAXARRAYLENGTH];
		Float_t mglobalPy[MAXARRAYLENGTH];
		Float_t mglobalPz[MAXARRAYLENGTH];
		Float_t mglobalEta[MAXARRAYLENGTH];
		Float_t mglobalPhi[MAXARRAYLENGTH];
		Short_t mglobalCharge[MAXARRAYLENGTH];
		UShort_t mglobalNHitsPos[MAXARRAYLENGTH];
		UShort_t mglobalNHitsFit[MAXARRAYLENGTH];
		Float_t mglobalDca[MAXARRAYLENGTH];			// Global dca, same as mgDca[] when primary track and global are the same id
		Float_t mglobalDcaD[MAXARRAYLENGTH];			// Global dca, same as mgDca[] when primary track and global are the same id
		Short_t mglobalid[MAXARRAYLENGTH];
		Double_t mglobaldEdx[MAXARRAYLENGTH];
		UShort_t mglobalNHitsdEdx[MAXARRAYLENGTH];
		Double_t mglobalNSigmaPion[MAXARRAYLENGTH];
		Double_t mglobalNSigmaKaon[MAXARRAYLENGTH];
		Double_t mglobalNSigmaProton[MAXARRAYLENGTH];






		ClassDef(pp12TreeMaker,1)		//	Macro for CINT compatability
};

#endif


		








