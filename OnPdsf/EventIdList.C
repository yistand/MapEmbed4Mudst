//====================================================================================================
//
//	2016.01.14	Li Yi
//	Read embedding root file one by one
//	Write out event id for each root as a list file. One list for each root.
//
//====================================================================================================
#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

using namespace std;

void ExtractId(string str, string &runid, string &fileid) {
	size_t pos = str.find("st_physics_adc_");
	pos=pos+15;
	runid = str.substr(pos,8);

	pos = str.find("raw_");
	pos=pos+4;
	fileid=str.substr(pos,7);	
}


void ExtractId(string str, string &runid, string &fileid, string &pathid, const string &particle) {
	size_t pos = str.find("st_physics_adc_");
	pos=pos+15;
	runid = str.substr(pos,8);

	pos = str.find("raw_");
	pos=pos+4;
	fileid=str.substr(pos,7);	

	pos = str.find(particle);
	pos=pos+particle.size()+1;
	pathid=str.substr(pos,3);
}

void ExtractParticle(const char* pathname, string &particle) {
	string str = string(pathname);
	size_t pos1 = str.find("lists/pp");
	pos1 = pos1+8;
	size_t pos2 = str.find("200.txt");
	particle = str.substr(pos1,pos2-pos1);
}

void EventIdList(char* inputfile =  "/global/u1/l/liyi/pp200embed/lists/ppPiplus200.txt") {

	ifstream infile;
	infile.open(inputfile,ios::in);

	string line;

	string particle;
	ExtractParticle(inputfile, particle);

	int mRunId;
	int mEventId;
	TFile *f;
	TTree *t;

	string runid;
	string fileid;
	string pathid;		// additional id in embedding root file path

	ofstream outfile;
	while(getline(infile, line)) {
		//cout<<"read "<<line<<endl;

		f = new TFile(line.data());
		if(f->IsZombie()) { cout<<"Skip"<<endl; continue;}

		ExtractId(line,runid,fileid,pathid,particle);
		outfile.open(Form("list/%s_%s_%s_%s.txt",runid.data(),fileid.data(),particle.data(),pathid.data()), ios::out);

		t = (TTree*)f->Get("StMiniMcTree");
		t->SetBranchAddress("mRunId", &mRunId);
		t->SetBranchAddress("mEventId", &mEventId);
		
		for(int ievt = 0; ievt<t->GetEntries(); ievt++)	{
			t->GetEntry(ievt);
			if(ievt==0) {
				//cout<<"Read: from file name runid "<<runid<<"? runid from tree "<<mRunId<<endl;
				outfile << mRunId << endl << endl ;
			}
			if(ievt!=t->GetEntries()-1) outfile << mEventId << endl ;
			else outfile << mEventId ;
		}
		f->Close();
		outfile.close();
	}

	infile.close();

}
