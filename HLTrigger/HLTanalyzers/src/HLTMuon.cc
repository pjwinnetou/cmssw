#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <functional>
#include <stdlib.h>
#include <string.h>

#include "HLTrigger/HLTanalyzers/interface/HLTMuon.h"


template <class T>
static inline
bool getTriggerFilterObjects( const edm::Event & event, const edm::Handle<trigger::TriggerFilterObjectWithRefs> & muonFilterCollection, edm::Handle<T> & product, edm::Handle<T> input) 
{
    if ( not muonFilterCollection.isValid()  && !muonFilterCollection.failedToGet()) return false;
    if ( not input.isValid() && !input.failedToGet() ) return false;
     
    std::vector< edm::InputTag > inputCollectionTags;
    muonFilterCollection->getCollectionTags(inputCollectionTags);
    if ( inputCollectionTags.size()==0 ) return false;

    for (unsigned int j=0; j<inputCollectionTags.size() ; j++) {
      edm::InputTag collectionTag = inputCollectionTags.at(j);
      if ( collectionTag.label()==input.provenance()->moduleLabel() && collectionTag.instance()==input.provenance()->productInstanceName() ) {
        event.getByLabel( collectionTag, product );
        if (product.isValid() && !product.failedToGet()) return true;
        else product.clear();
      }
    }
    
    return false;
}


HLTMuon::HLTMuon() {
  evtCounter=0;

  //set parameter defaults 
  _Monte=false;
  _Debug=false;
}

/*  Setup the analysis to put the branch-variables into the tree. */
void HLTMuon::setup(const edm::ParameterSet& pSet, TTree* HltTree) {

  edm::ParameterSet myEmParams = pSet.getParameter<edm::ParameterSet>("RunParameters") ;
  std::vector<std::string> parameterNames = myEmParams.getParameterNames() ;

  for ( std::vector<std::string>::iterator iParam = parameterNames.begin();
	iParam != parameterNames.end(); iParam++ ){
    if  ( (*iParam) == "Monte" ) _Monte =  myEmParams.getParameter<bool>( *iParam );
    else if ( (*iParam) == "Debug" ) _Debug =  myEmParams.getParameter<bool>( *iParam );
  }

  hltOnlineBeamSpot = TVector3(0.0,0.0,0.0);

  const int kMaxMuon = 10000;
  muonReco_pt = new float[kMaxMuon];
  muonReco_phi = new float[kMaxMuon];
  muonReco_eta = new float[kMaxMuon];
  muonReco_et = new float[kMaxMuon];
  muonReco_e = new float[kMaxMuon];
  muonReco_chi2NDF = new float[kMaxMuon];
  muonReco_charge = new int[kMaxMuon];
  muonReco_D0 = new float[kMaxMuon];
  muonReco_type = new int[kMaxMuon];
  muonReco_NValidTrkHits = new int[kMaxMuon];
  muonReco_NValidMuonHits = new int[kMaxMuon];

  const int kMaxMuonL1 = 10000;
  muonL1_pt = new float[kMaxMuonL1];
  muonL1_phi = new float[kMaxMuonL1];
  muonL1_eta = new float[kMaxMuonL1];
  muonL1_bx = new int[kMaxMuonL1];
  muonL1_GMTMuonQuality = new int[kMaxMuonL1];
  muonL1_charge = new int[kMaxMuonL1];
  muonL1_trig = new ULong64_t[kMaxMuonL1];
  muonL1_tfMuonIndex = new int[kMaxMuonL1];
  muonL1_tfRegion = new int[kMaxMuonL1];

  const int kMaxMuonL2 = 10000;
  muonL2_pt = new float[kMaxMuonL2];
  muonL2_phi = new float[kMaxMuonL2];
  muonL2_eta = new float[kMaxMuonL2];
  muonL2_dr = new float[kMaxMuonL2];
  muonL2_L1dr = new float[kMaxMuonL2];
  muonL2_drsign = new float[kMaxMuonL2];
  muonL2_dz = new float[kMaxMuonL2];
  muonL2_vtxz = new float[kMaxMuonL2];
  muonL2_charge = new int[kMaxMuonL2];
  muonL2_pterr = new float[kMaxMuonL2];
  muonL2_nhits = new int[kMaxMuonL2];
  muonL2_nchambers = new int[kMaxMuonL2]; 
  muonL2_nstat = new int[kMaxMuonL2]; 
  muonL2_ndtcscstat = new int[kMaxMuonL2]; 
  muonL2_L1idx = new int[kMaxMuonL2];
  muonL2_trig = new ULong64_t[kMaxMuonL2];

  const int kMaxMuonL3 = 10000;
  muonL3_pt = new float[kMaxMuonL3];
  muonL3_ptLx = new float[kMaxMuonL3];
  muonL3_phi = new float[kMaxMuonL3];
  muonL3_eta = new float[kMaxMuonL3];
  muonL3_TrackL2dr = new float[kMaxMuonL3];
  muonL3_L2dr = new float[kMaxMuonL3];
  muonL3_L1dr = new float[kMaxMuonL3];
  muonL3_dr = new float[kMaxMuonL3];
  muonL3_dz = new float[kMaxMuonL3];
  muonL3_vtxz = new float[kMaxMuonL3];
  muonL3_charge = new int[kMaxMuonL3];
  muonL3_pterr = new float[kMaxMuonL3];
  muonL3_nhits = new int[kMaxMuonL3];
  muonL3_normchi2 = new float[kMaxMuonL3];
  muonL3_npixelhits = new int[kMaxMuonL3];
  muonL3_ntrackerhits = new int[kMaxMuonL3];
  muonL3_nmuonhits = new int[kMaxMuonL3];
  muonL3_L2idx = new int[kMaxMuonL3];
  muonL3_globalpt = new float[kMaxMuonL3]; 
  muonL3_globaleta = new float[kMaxMuonL3];
  muonL3_globalphi = new float[kMaxMuonL3];
  muonL3_globalDxy = new float[kMaxMuonL3];
  muonL3_globalDxySig = new float[kMaxMuonL3];
  muonL3_globaldz = new float[kMaxMuonL3];
  muonL3_globalvtxz = new float[kMaxMuonL3];
  muonL3_globalchg = new int[kMaxMuonL3];
  muonL3_global2idx = new int[kMaxMuonL3];
  muonL3_trig = new ULong64_t[kMaxMuonL3];

  const int kMaxL3TkL2OIState = 10000;
  L3TkL2OIState_pt = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_phi = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_eta = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_dr = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_drError = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_dz = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_charge = new int[kMaxL3TkL2OIState];
  L3TkL2OIState_nhits = new int[kMaxL3TkL2OIState];
  L3TkL2OIState_normchi2 = new float[kMaxL3TkL2OIState];
  L3TkL2OIState_npixelhits = new int[kMaxL3TkL2OIState];
  L3TkL2OIState_ntrackerhits = new int[kMaxL3TkL2OIState];
  L3TkL2OIState_L3idx = new int[kMaxL3TkL2OIState];
  L3TkL2OIState_L2idx = new int[kMaxL3TkL2OIState];

  const int kMaxL3TkL2OIHit = 10000;
  L3TkL2OIHit_pt = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_phi = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_eta = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_dr = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_drError = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_dz = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_charge = new int[kMaxL3TkL2OIHit];
  L3TkL2OIHit_nhits = new int[kMaxL3TkL2OIHit];
  L3TkL2OIHit_normchi2 = new float[kMaxL3TkL2OIHit];
  L3TkL2OIHit_npixelhits = new int[kMaxL3TkL2OIHit];
  L3TkL2OIHit_ntrackerhits = new int[kMaxL3TkL2OIHit];
  L3TkL2OIHit_L3idx = new int[kMaxL3TkL2OIHit];
  L3TkL2OIHit_L2idx = new int[kMaxL3TkL2OIHit];

  const int kMaxDiMuL1 = 10000;
  dimuonL1_charge = new int[kMaxDiMuL1];
  dimuonL1_invmass = new float[kMaxDiMuL1];
  dimuonL1_pt = new float[kMaxDiMuL1];
  dimuonL1_rap = new float[kMaxDiMuL1];
  dimuonL1_Mu1idx = new int[kMaxDiMuL1];
  dimuonL1_Mu2idx = new int[kMaxDiMuL1];
  dimuonL1_trig = new ULong64_t[kMaxDiMuL1];
  dimuonL1_dR = new float[kMaxDiMuL1];
  dimuonL1_phi = new float[kMaxDiMuL1];

  const int kMaxDiMuL2 = 10000;
  dimuonL2_charge = new int[kMaxDiMuL2];
  dimuonL2_invmass = new float[kMaxDiMuL2];
  dimuonL2_pt = new float[kMaxDiMuL2];
  dimuonL2_rap = new float[kMaxDiMuL2];
  dimuonL2_dPhi = new float[kMaxDiMuL2];
  dimuonL2_angle = new float[kMaxDiMuL2];
  dimuonL2_dPt = new float[kMaxDiMuL2];
  dimuonL2_Mu1idx = new int[kMaxDiMuL2];
  dimuonL2_Mu2idx = new int[kMaxDiMuL2];
  dimuonL2_trig = new ULong64_t[kMaxDiMuL2];

  const int kMaxDiMuL3 = 10000;
  dimuonL3_dca = new float[kMaxDiMuL3];
  dimuonL3_charge = new int[kMaxDiMuL3];
  dimuonL3_invmass = new float[kMaxDiMuL3];
  dimuonL3_pt = new float[kMaxDiMuL3];
  dimuonL3_rap = new float[kMaxDiMuL3];
  dimuonL3_dPhi = new float[kMaxDiMuL3];
  dimuonL3_dPt = new float[kMaxDiMuL3];
  dimuonL3_isCowboy = new int[kMaxDiMuL3];
  dimuonL3_Mu1idx = new int[kMaxDiMuL3];
  dimuonL3_Mu2idx = new int[kMaxDiMuL3];
  dimuonL3_trig = new ULong64_t[kMaxDiMuL3];

  // Muon-specific branches of the tree 
  HltTree->Branch("hltOnlineBeamSpot",      "TVector3",              &hltOnlineBeamSpot);
  HltTree->Branch("MuonReco_N",             &nmuonReco,               "MuonReco_N/I");
  HltTree->Branch("MuonReco_Pt",             muonReco_pt,             "MuonReco_Pt[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Phi",            muonReco_phi,            "MuonReco_Phi[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Eta",            muonReco_eta,            "MuonReco_Eta[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Et",             muonReco_et,             "MuonReco_Et[MuonReco_N]/F");
  HltTree->Branch("MuonReco_E",              muonReco_e,              "MuonReco_E[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Chi2NDF",        muonReco_chi2NDF,        "MuonReco_Chi2NDF[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Charge",         muonReco_charge  ,       "MuonReco_Charge[MuonReco_N]/I");
  HltTree->Branch("MuonReco_D0",             muonReco_D0 , 	      "MuonReco_D0[MuonReco_N]/F");
  HltTree->Branch("MuonReco_Type",           muonReco_type       ,    "MuonReco_Type[MuonReco_N]/I");
  HltTree->Branch("MuonReco_NValidTrkHits",  muonReco_NValidTrkHits,  "MuonReco_NValidTrkHits[MuonReco_N]/I");
  HltTree->Branch("MuonReco_NValidMuonHits", muonReco_NValidMuonHits, "MuonReco_NValidMuonHits[MuonReco_N]/I");

  HltTree->Branch("MuonL1_N",              &nmuonL1,                "MuonL1_N/I");
  HltTree->Branch("MuonL1_trig",            muonL1_trig,            "MuonL1_trig[MuonL1_N]/l");
  HltTree->Branch("MuonL1_Pt",              muonL1_pt,              "MuonL1_Pt[MuonL1_N]/F");
  HltTree->Branch("MuonL1_Phi",             muonL1_phi,             "MuonL1_Phi[MuonL1_N]/F");
  HltTree->Branch("MuonL1_Eta",             muonL1_eta,             "MuonL1_Eta[MuonL1_N]/F");
  HltTree->Branch("MuonL1_Charge",          muonL1_charge,          "MuonL1_Charge[MuonL1_N]/I");
  HltTree->Branch("MuonL1_Bx",              muonL1_bx,              "MuonL1_Bx[MuonL1_N]/I");
  HltTree->Branch("MuonL1_GMTMuonQuality",  muonL1_GMTMuonQuality,  "MuonL1_GMTMuonQuality[MuonL1_N]/I");
  HltTree->Branch("MuonL1_tfMuonIndex",     muonL1_tfMuonIndex,     "MuonL1_tfMuonIndex[MuonL1_N]/I");
  HltTree->Branch("MuonL1_tfRegion",        muonL1_tfRegion,        "MuonL1_tfRegion[MuonL1_N]/I");

  HltTree->Branch("MuonL2_N",           &nmuonL2,          "MuonL2_N/I");
  HltTree->Branch("MuonL2_trig",        muonL2_trig,       "MuonL2_trig[MuonL2_N]/l");
  HltTree->Branch("MuonL2_Pt",          muonL2_pt,         "MuonL2_Pt[MuonL2_N]/F");
  HltTree->Branch("MuonL2_Phi",         muonL2_phi,        "MuonL2_Phi[MuonL2_N]/F");
  HltTree->Branch("MuonL2_Eta",         muonL2_eta,        "MuonL2_Eta[MuonL2_N]/F");
  HltTree->Branch("MuonL2_Charge",      muonL2_charge,     "MuonL2_Charge[MuonL2_N]/I");
  HltTree->Branch("MuonL2_PtErr",       muonL2_pterr,      "MuonL2_PtErr[MuonL2_N]/F");
  HltTree->Branch("MuonL2_L2vsL1_Dr",   muonL2_L1dr,       "MuonL2_L2vsL1_Dr[MuonL2_N]/F");
  HltTree->Branch("MuonL2_L2vsBS_Dr",   muonL2_dr,         "MuonL2_L2vsBS_Dr[MuonL2_N]/F");
  HltTree->Branch("MuonL2_DrSign",      muonL2_drsign,     "MuonL2_DrSign[MuonL2_N]/F");
  HltTree->Branch("MuonL2_Dz",          muonL2_dz,         "MuonL2_Dz[MuonL2_N]/F");
  HltTree->Branch("MuonL2_VtxZ",        muonL2_vtxz,       "MuonL2_VtxZ[MuonL2_N]/F");
  HltTree->Branch("MuonL2_Nhits",       muonL2_nhits,      "MuonL2_Nhits[MuonL2_N]/I");
  HltTree->Branch("MuonL2_Nchambers",   muonL2_nchambers,  "MuonL2_Nchambers[MuonL2_N]/I");   
  HltTree->Branch("MuonL2_Nstat",       muonL2_nstat,      "MuonL2_Nstat[MuonL2_N]/I");   
  HltTree->Branch("MuonL2_NDtCscStat",  muonL2_ndtcscstat, "MuonL2_NDtCscStat[MuonL2_N]/I");   
  HltTree->Branch("MuonL2_L1idx",       muonL2_L1idx,      "MuonL2_L1idx[MuonL2_N]/I");
   
  HltTree->Branch("MuonL3_N",           &nmuonL3,             "MuonL3_N/I");
  HltTree->Branch("MuonL3_trig",         muonL3_trig,         "MuonL3_trig[MuonL3_N]/l");
  HltTree->Branch("MuonL3_Pt",           muonL3_pt,           "MuonL3_Pt[MuonL3_N]/F");
  HltTree->Branch("MuonL3_PtLx",         muonL3_ptLx,         "MuonL3_PtLx[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Phi",          muonL3_phi,          "MuonL3_Phi[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Eta",          muonL3_eta,          "MuonL3_Eta[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Charge",       muonL3_charge,       "MuonL3_Charge[MuonL3_N]/I");
  HltTree->Branch("MuonL3_PtErr",        muonL3_pterr,        "MuonL3_PtErr[MuonL3_N]/F");
  HltTree->Branch("MuonL3_TrackvsL2_Dr", muonL3_TrackL2dr,    "MuonL3_TrackvsL2_Dr[MuonL3_N]/F");
  HltTree->Branch("MuonL3_L3vsL1_Dr",    muonL3_L1dr,         "MuonL3_L3vsL1_Dr[MuonL3_N]/F");
  HltTree->Branch("MuonL3_L3vsL2_Dr",    muonL3_L2dr,         "MuonL3_L3vsL2_Dr[MuonL3_N]/F");
  HltTree->Branch("MuonL3_L3vsBS_Dr",    muonL3_dr,           "MuonL3_L3vsBS_Dr[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Dz",           muonL3_dz,           "MuonL3_Dz[MuonL3_N]/F");
  HltTree->Branch("MuonL3_VtxZ",         muonL3_vtxz,         "MuonL3_VtxZ[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Nhits",        muonL3_nhits,        "MuonL3_Nhits[MuonL3_N]/I");    
  HltTree->Branch("MuonL3_NormChi2",     muonL3_normchi2,     "MuonL3_NormChi2[MuonL3_N]/F");
  HltTree->Branch("MuonL3_Npixelhits",   muonL3_npixelhits,   "MuonL3_Npixelhits[MuonL3_N]/I"); 
  HltTree->Branch("MuonL3_Ntrackerhits", muonL3_ntrackerhits, "MuonL3_Ntrackerhits[MuonL3_N]/I"); 
  HltTree->Branch("MuonL3_Nmuonhits",    muonL3_nmuonhits,    "MuonL3_Nmuonhits[MuonL3_N]/I"); 
  HltTree->Branch("MuonL3_L2idx",        muonL3_L2idx,        "MuonL3_L2idx[MuonL3_N]/I");
  HltTree->Branch("MuonL3_globalPt",     muonL3_globalpt,     "MuonL3_globalPt[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalEta",    muonL3_globaleta,    "MuonL3_globalEta[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalPhi",    muonL3_globalphi,    "MuonL3_globalPhi[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalDxy",    muonL3_globalDxy,    "MuonL3_globalDxy[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalDxySig", muonL3_globalDxySig, "MuonL3_globalDxySig[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalDz",     muonL3_globaldz,     "MuonL3_globalDz[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalVtxZ",   muonL3_globalvtxz,   "MuonL3_globalVtxZ[MuonL3_N]/F");
  HltTree->Branch("MuonL3_globalL2idx",  muonL3_global2idx,   "MuonL3_globalL2idx[MuonL3_N]/I");

  HltTree->Branch("DiMuonL1_N",       &nDiMuonL1,         "DiMuonL1_N/I");
  HltTree->Branch("DiMuonL1_trig",     dimuonL1_trig,     "DiMuonL1_trig[DiMuonL1_N]/l");
  HltTree->Branch("DiMuonL1_InvMass",  dimuonL1_invmass,  "DiMuonL1_InvMass[DiMuonL1_N]/F");
  HltTree->Branch("DiMuonL1_Pt",       dimuonL1_pt,       "DiMuonL1_Pt[DiMuonL1_N]/F");
  HltTree->Branch("DiMuonL1_Rap",      dimuonL1_rap,      "DiMuonL1_Rap[DiMuonL1_N]/F");   
  HltTree->Branch("DiMuonL1_Charge",   dimuonL1_charge,   "DiMuonL1_Charge[DiMuonL1_N]/I");
  HltTree->Branch("DiMuonL1_Mu1idx",   dimuonL1_Mu1idx,   "DiMuonL1_Mu1idx[DiMuonL1_N]/I");    
  HltTree->Branch("DiMuonL1_Mu2idx",   dimuonL1_Mu2idx,   "DiMuonL1_Mu2idx[DiMuonL1_N]/I");
  HltTree->Branch("DiMuonL1_dR",       dimuonL1_dR,       "DiMuonL1_dR[DiMuonL1_N]/F");  
  HltTree->Branch("DiMuonL1_phi",      dimuonL1_phi,      "DiMuonL1_phi[DiMuonL1_N]/F");  

  HltTree->Branch("DiMuonL2_N",       &nDiMuonL2,         "DiMuonL2_N/I");
  HltTree->Branch("DiMuonL2_trig",     dimuonL2_trig,     "DiMuonL2_trig[DiMuonL2_N]/l");
  HltTree->Branch("DiMuonL2_InvMass",  dimuonL2_invmass,  "DiMuonL2_InvMass[DiMuonL2_N]/F");
  HltTree->Branch("DiMuonL2_Pt",       dimuonL2_pt,       "DiMuonL2_Pt[DiMuonL2_N]/F");
  HltTree->Branch("DiMuonL2_Rap",      dimuonL2_rap,      "DiMuonL2_Rap[DiMuonL2_N]/F");   
  HltTree->Branch("DiMuonL2_Charge",   dimuonL2_charge,   "DiMuonL2_Charge[DiMuonL2_N]/I");
  HltTree->Branch("DiMuonL2_dPt",      dimuonL2_dPt,      "DiMuonL2_dPt[DiMuonL2_N]/F");
  HltTree->Branch("DiMuonL2_dPhi",     dimuonL2_dPhi,     "DiMuonL2_dPhi[DiMuonL2_N]/F");
  HltTree->Branch("DiMuonL2_Angle",    dimuonL2_angle,    "DiMuonL2_Angle[DiMuonL2_N]/F");
  HltTree->Branch("DiMuonL2_Mu1idx",   dimuonL2_Mu1idx,   "DiMuonL2_Mu1idx[DiMuonL2_N]/I");    
  HltTree->Branch("DiMuonL2_Mu2idx",   dimuonL2_Mu2idx,   "DiMuonL2_Mu2idx[DiMuonL2_N]/I");

  HltTree->Branch("DiMuonL3_N",       &nDiMuonL3,         "DiMuonL3_N/I");
  HltTree->Branch("DiMuonL3_trig",     dimuonL3_trig,     "DiMuonL3_trig[DiMuonL3_N]/l");
  HltTree->Branch("DiMuonL3_InvMass",  dimuonL3_invmass,  "DiMuonL3_InvMass[DiMuonL3_N]/F");
  HltTree->Branch("DiMuonL3_Pt",       dimuonL3_pt,       "DiMuonL3_Pt[DiMuonL3_N]/F");
  HltTree->Branch("DiMuonL3_Rap",      dimuonL3_rap,      "DiMuonL3_Rap[DiMuonL3_N]/F");   
  HltTree->Branch("DiMuonL3_Charge",   dimuonL3_charge,   "DiMuonL3_Charge[DiMuonL3_N]/I");
  HltTree->Branch("DiMuonL3_DCA",      dimuonL3_dca,      "DiMuonL3_DCA[DiMuonL3_N]/F");
  HltTree->Branch("DiMuonL3_dPt",      dimuonL3_dPt,      "DiMuonL3_dPt[DiMuonL3_N]/F");
  HltTree->Branch("DiMuonL3_dPhi",     dimuonL3_dPhi,     "DiMuonL3_dPhi[DiMuonL3_N]/F");
  HltTree->Branch("DiMuonL3_isCowboy", dimuonL3_isCowboy, "DiMuonL3_isCowboy[DiMuonL3_N]/I");
  HltTree->Branch("DiMuonL3_Mu1idx",   dimuonL3_Mu1idx,   "DiMuonL3_Mu1idx[DiMuonL3_N]/I");    
  HltTree->Branch("DiMuonL3_Mu2idx",   dimuonL3_Mu2idx,   "DiMuonL3_Mu2idx[DiMuonL3_N]/I");

  HltTree->Branch("L3TkL2OIHit_N",             &nL3TkL2OIHit,             "L3TkL2OIHit_N/I");
  HltTree->Branch("L3TkL2OIHit_Pt",             L3TkL2OIHit_pt,           "L3TkL2OIHit_Pt[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Phi",            L3TkL2OIHit_phi,          "L3TkL2OIHit_Phi[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Eta",            L3TkL2OIHit_eta,          "L3TkL2OIHit_Eta[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Charge",         L3TkL2OIHit_charge,       "L3TkL2OIHit_Charge[L3TkL2OIHit_N]/I");
  HltTree->Branch("L3TkL2OIHit_L3vsBS_Dr",      L3TkL2OIHit_dr,           "L3TkL2OIHit_L3vsBS_Dr[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_L3vsBS_DrError", L3TkL2OIHit_drError,      "L3TkL2OIHit_L3vsBS_DrError[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Dz",             L3TkL2OIHit_dz,           "L3TkL2OIHit_Dz[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Nhits",          L3TkL2OIHit_nhits,        "L3TkL2OIHit_Nhits[L3TkL2OIHit_N]/I");    
  HltTree->Branch("L3TkL2OIHit_NormChi2",       L3TkL2OIHit_normchi2,     "L3TkL2OIHit_NormChi2[L3TkL2OIHit_N]/F");
  HltTree->Branch("L3TkL2OIHit_Npixelhits",     L3TkL2OIHit_npixelhits,   "L3TkL2OIHit_Npixelhits[L3TkL2OIHit_N]/I"); 
  HltTree->Branch("L3TkL2OIHit_Ntrackerhits",   L3TkL2OIHit_ntrackerhits, "L3TkL2OIHit_Ntrackerhits[L3TkL2OIHit_N]/I");
  HltTree->Branch("L3TkL2OIHit_L3idx",          L3TkL2OIHit_L3idx,        "L3TkL2OIHit_L3idx[L3TkL2OIHit_N]/I");
  HltTree->Branch("L3TkL2OIHit_L2idx",          L3TkL2OIHit_L2idx,        "L3TkL2OIHit_L2idx[L3TkL2OIHit_N]/I");


  HltTree->Branch("L3TkL2OIState_N",             &nL3TkL2OIState,             "L3TkL2OIState_N/I");
  HltTree->Branch("L3TkL2OIState_Pt",             L3TkL2OIState_pt,           "L3TkL2OIState_Pt[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Phi",            L3TkL2OIState_phi,          "L3TkL2OIState_Phi[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Eta",            L3TkL2OIState_eta,          "L3TkL2OIState_Eta[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Charge",         L3TkL2OIState_charge,       "L3TkL2OIState_Charge[L3TkL2OIState_N]/I");
  HltTree->Branch("L3TkL2OIState_L3vsBS_Dr",      L3TkL2OIState_dr,           "L3TkL2OIState_L3vsBS_Dr[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_L3vsBS_DrError", L3TkL2OIState_drError,      "L3TkL2OIState_L3vsBS_DrError[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Dz",             L3TkL2OIState_dz,           "L3TkL2OIState_Dz[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Nhits",          L3TkL2OIState_nhits,        "L3TkL2OIState_Nhits[L3TkL2OIState_N]/I");    
  HltTree->Branch("L3TkL2OIState_NormChi2",       L3TkL2OIState_normchi2,     "L3TkL2OIState_NormChi2[L3TkL2OIState_N]/F");
  HltTree->Branch("L3TkL2OIState_Npixelhits",     L3TkL2OIState_npixelhits,   "L3TkL2OIState_Npixelhits[L3TkL2OIState_N]/I"); 
  HltTree->Branch("L3TkL2OIState_Ntrackerhits",   L3TkL2OIState_ntrackerhits, "L3TkL2OIState_Ntrackerhits[L3TkL2OIState_N]/I");
  HltTree->Branch("L3TkL2OIState_L3idx",          L3TkL2OIState_L3idx,        "L3TkL2OIState_L3idx[L3TkL2OIState_N]/I");
  HltTree->Branch("L3TkL2OIState_L2idx",          L3TkL2OIState_L2idx,        "L3TkL2OIState_L2idx[L3TkL2OIState_N]/I");

}

/* **Analyze the event** */
void HLTMuon::analyze(const edm::Event & event,
                      const edm::Handle<reco::MuonCollection>                      & Muon,
		      const edm::Handle< BXVector<l1t::Muon> >                     & MuCands1, 
		      const edm::Handle<reco::RecoChargedCandidateCollection>      & MuCands2,
		      const edm::Handle<reco::RecoChargedCandidateCollection>      & MuCands3,
                      const edm::Handle<std::vector<reco::Track>>                  & L3TkTracksFromL2OIState,
                      const edm::Handle<std::vector<reco::Track>>                  & L3TkTracksFromL2OIHit,
                      const std::vector< edm::Handle<trigger::TriggerFilterObjectWithRefs> > & muonFilterCollections,
		      const edm::ESHandle<MagneticField> & theMagField,
		      const edm::Handle<reco::BeamSpot> & recoBeamSpotHandle,
		      TTree* HltTree) {
  
  reco::BeamSpot::Point BSPosition(0,0,0);
  if (recoBeamSpotHandle.isValid() && !recoBeamSpotHandle.failedToGet()) {
    BSPosition = recoBeamSpotHandle->position();
  }
  hltOnlineBeamSpot.SetXYZ(BSPosition.x(),BSPosition.y(),BSPosition.z());
  
  //std::cout << " Beginning HLTMuon " << std::endl;

  if (Muon.isValid() && !Muon.failedToGet()) {
    reco::MuonCollection mymuons;
    mymuons = * Muon;
    std::sort(mymuons.begin(),mymuons.end(),PtGreater());
    nmuonReco = mymuons.size();
    typedef reco::MuonCollection::const_iterator muiter;

    int imu=0;
    for (muiter i=mymuons.begin(); i!=mymuons.end(); i++) 
      {
	muonReco_pt[imu]         = i->pt();
	muonReco_phi[imu]        = i->phi();
	muonReco_eta[imu]        = i->eta();
	muonReco_et[imu]         = i->et();
	muonReco_e[imu]          = i->energy(); 
	muonReco_type[imu]       = i->type();
	muonReco_charge[imu]     = i->charge(); 

	if (i->globalTrack().isNonnull())
	  {
	    muonReco_chi2NDF[imu] = i->globalTrack()->normalizedChi2();
	    muonReco_D0[imu] = i->globalTrack()->dxy(BSPosition);
	  }
	else 
	  {
	    muonReco_chi2NDF[imu] = -99.;
	    muonReco_D0[imu] = -99.;}

	if (i->innerTrack().isNonnull()) muonReco_NValidTrkHits[imu] = i->innerTrack()->numberOfValidHits();
	else muonReco_NValidTrkHits[imu] = -99;

	if (i->isGlobalMuon()!=0) muonReco_NValidMuonHits[imu] = i->globalTrack()->hitPattern().numberOfValidMuonHits();
	else muonReco_NValidMuonHits[imu] = -99;

	imu++;
      }
  }
  else {nmuonReco = 0;}
  
  std::vector< edm::Handle< BXVector<l1t::Muon> > >                MuCands1FilterCollection; 
  std::vector< edm::Handle<reco::RecoChargedCandidateCollection> > MuCands2FilterCollection;
  std::vector< edm::Handle<reco::RecoChargedCandidateCollection> > MuCands3FilterCollection;
  std::vector< edm::Handle< std::vector< reco::Track > > >         L3TkTracksFromL2OIStateFilterCollection;
  std::vector< edm::Handle< std::vector< reco::Track > > >         L3TkTracksFromL2OIHitFilterCollection;

  std::vector< std::string > filterNames;
  for (unsigned int i=0; i<muonFilterCollections.size(); i++) {
    edm::Handle<trigger::TriggerFilterObjectWithRefs> muonFilterCollection = muonFilterCollections.at(i);
        
    edm::Handle< BXVector<l1t::Muon> >                  MuCands1Tmp;
    getTriggerFilterObjects( event, muonFilterCollection, MuCands1Tmp, MuCands1);
    MuCands1FilterCollection.push_back( MuCands1Tmp );
    
    edm::Handle<reco::RecoChargedCandidateCollection>   MuCands2Tmp;
    getTriggerFilterObjects( event, muonFilterCollection, MuCands2Tmp, MuCands2);
    MuCands2FilterCollection.push_back( MuCands2Tmp);
    
    edm::Handle<reco::RecoChargedCandidateCollection>   MuCands3Tmp; 
    getTriggerFilterObjects( event, muonFilterCollection, MuCands3Tmp, MuCands3);
    MuCands3FilterCollection.push_back(MuCands3Tmp);
    
  }
      
      
  /////////////////////////////// Open-HLT muons ///////////////////////////////

  // Dealing with L1 muons
  if (MuCands1.isValid() && !MuCands1.failedToGet()) {
    int imu1c=0;
    typedef std::vector<l1t::Muon>::const_iterator cand;
    int idimu1c=0;
    for(int i = MuCands1->getFirstBX(); i <= MuCands1->getLastBX(); ++i) { 
      for (cand l1=MuCands1->begin(i); l1!=MuCands1->end(i); ++l1) {
        ULong64_t trigBits = 0;
        for (unsigned int j=0; j<MuCands1FilterCollection.size(); j++) {
          edm::Handle< BXVector<l1t::Muon> >  MuCands1Tmp = MuCands1FilterCollection.at(j);
          if (MuCands1Tmp.isValid() && !MuCands1Tmp.failedToGet()) { 
            bool found = false;
            for(int u = MuCands1Tmp->getFirstBX(); u <= MuCands1Tmp->getLastBX(); ++u) { 
              for (cand kl1=MuCands1Tmp->begin(u); kl1!=MuCands1Tmp->end(u); ++kl1) {
                if ( (l1->pt()  == kl1->pt())   && 
                     (l1->eta() == kl1->eta())  && 
                     (l1->phi() == kl1->phi())  &&
                     (l1->charge() == kl1->charge())  &&
                     (l1->hwQual() == kl1->hwQual())
                     ){
                  trigBits += (ULong64_t)pow(2, j); 
                  found = true;
                  break;
                }
              }
              if (found==true) { break; }
            }
          }
        }
        muonL1_trig[imu1c] = trigBits;
        
        muonL1_pt[imu1c] = l1->pt();
        muonL1_eta[imu1c] = l1->eta();
        muonL1_phi[imu1c] = l1->phi();
        muonL1_bx[imu1c] = i;
        muonL1_GMTMuonQuality[imu1c] = l1->hwQual();
        muonL1_charge[imu1c] = l1->charge();

        muonL1_tfMuonIndex[imu1c] = l1->tfMuonIndex();
        muonL1_tfRegion[imu1c] = -1;
        if ( l1->tfMuonIndex()>35 && l1->tfMuonIndex()<72 ) 
          {
            muonL1_tfRegion[imu1c] = 0; // Barrel Muon Track Finder 
          }
        else if ( (l1->tfMuonIndex()>17 && l1->tfMuonIndex()<36) || (l1->tfMuonIndex()>71 && l1->tfMuonIndex()<90) )
          {
            muonL1_tfRegion[imu1c] = 1; // Overlap Muon Track Finder 
          }
        else if ( (l1->tfMuonIndex()<18) || (l1->tfMuonIndex()>89) ) 
          {
            muonL1_tfRegion[imu1c] = 2; // Endcap Muon Track Finder 
          }
         
        int imu1c2nd = imu1c + 1;// This will be the index in the hltTree for the 2nd muon of the dimuon combination

        for (cand l1_2nd=l1; l1_2nd!=MuCands1->end(i); l1_2nd++) if (l1!=l1_2nd) {//Loop over all L1 muons from the one we are already treating

            ULong64_t trigBits2nd=0;
            for (unsigned int j2nd=0; j2nd<MuCands1FilterCollection.size(); j2nd++) {
              edm::Handle< BXVector<l1t::Muon> >  MuCands1Tmp2nd = MuCands1FilterCollection.at(j2nd);
              if (MuCands1Tmp2nd.isValid() && !MuCands1Tmp2nd.failedToGet()) { 
                bool found = false;
                for(int u2nd = MuCands1Tmp2nd->getFirstBX(); u2nd <= MuCands1Tmp2nd->getLastBX(); ++u2nd) {
                  for (cand kl1_2nd=MuCands1Tmp2nd->begin(u2nd); kl1_2nd!=MuCands1Tmp2nd->end(u2nd); ++kl1_2nd) {
                    if ( (l1_2nd->pt()  == kl1_2nd->pt())   && 
                         (l1_2nd->eta() == kl1_2nd->eta())  && 
                         (l1_2nd->phi() == kl1_2nd->phi())  &&
                         (l1_2nd->charge() == kl1_2nd->charge())  &&
                         (l1_2nd->hwQual() == kl1_2nd->hwQual())
                         ){
                      trigBits2nd += (ULong64_t)pow(2, j2nd);
                      found = true;
                      break;
                    }
                  }
                  if (found==true) { break; }
                }
              }
            }
            dimuonL1_trig[idimu1c] = trigBits & trigBits2nd;   

            dimuonL1_Mu1idx[idimu1c] = imu1c;
            dimuonL1_Mu2idx[idimu1c] = imu1c2nd;
            dimuonL1_charge[idimu1c] = l1->charge() + l1_2nd->charge();
      
            // Combined dimuon system
            double const MuMass = 0.106;
            double const MuMass2 = MuMass*MuMass;
            double e1 = sqrt(l1->momentum().Mag2()+MuMass2);
            double e2 = sqrt(l1_2nd->momentum().Mag2()+MuMass2);
            reco::Particle::LorentzVector p1 = reco::Particle::LorentzVector(l1->px(),l1->py(),l1->pz(),e1);
            reco::Particle::LorentzVector p2 = reco::Particle::LorentzVector(l1_2nd->px(),l1_2nd->py(),l1_2nd->pz(),e2);
            reco::Particle::LorentzVector p = p1+p2;

            dimuonL1_invmass[idimu1c] = p.mass();
            dimuonL1_pt[idimu1c] = p.pt();
            dimuonL1_rap[idimu1c] = p.Rapidity();

            Double_t deta = p1.Eta()-p2.Eta();
            Double_t dphi = TVector2::Phi_mpi_pi(p1.Phi()-p2.Phi());
            dimuonL1_dR[idimu1c] = TMath::Sqrt( deta*deta+dphi*dphi );
            dimuonL1_phi[idimu1c] = p.Phi();

            imu1c2nd++;
            idimu1c++;
          }

        imu1c++;
      }
    }
    nmuonL1 = imu1c;
    nDiMuonL1 = idimu1c;
  }
  else {nmuonL1 = 0; nDiMuonL1 = 0;}

  // Dealing with L2 muons
  reco::RecoChargedCandidateCollection myMucands2;
  if (MuCands2.isValid() && !MuCands2.failedToGet()) {
    //     reco::RecoChargedCandidateCollection myMucands2;
    myMucands2 = * MuCands2;
    std::sort(myMucands2.begin(),myMucands2.end(),PtGreater());
    nmuonL2 = myMucands2.size();
    typedef reco::RecoChargedCandidateCollection::const_iterator cand;
    int imu2c=0;
    int idimu2c=0;
    for (cand i=myMucands2.begin(); i!=myMucands2.end(); i++) {
      reco::TrackRef tk = i->get<reco::TrackRef>();
      
      ULong64_t trigBits=0;
      for (unsigned int j=0; j<MuCands2FilterCollection.size(); j++) {
        edm::Handle<reco::RecoChargedCandidateCollection>  MuCands2Tmp = MuCands2FilterCollection.at(j);
        if (MuCands2Tmp.isValid() && !MuCands2Tmp.failedToGet()) {
          for (cand k=MuCands2Tmp->begin(); k!=MuCands2Tmp->end(); k++) {
            reco::TrackRef tkFiltered = k->get<reco::TrackRef>();
            if ( tkFiltered==tk ) {
              trigBits += (ULong64_t) pow(2, j);
              break;
            }
          }
        }
      }
      muonL2_trig[imu2c] = trigBits;      

      muonL2_pt[imu2c] = tk->pt();
      // eta (we require |eta|<2.5 in all filters
      muonL2_eta[imu2c] = tk->eta();
      muonL2_phi[imu2c] = tk->phi();

      // Dr (transverse distance to (0,0,0))
      // For baseline triggers, we do no cut at L2 (|dr|<9999 cm)
      // However, we use |dr|<200 microns at L3, which it probably too tough for LHC startup
      muonL2_dr[imu2c] = fabs(tk->dxy(BSPosition));
      muonL2_drsign[imu2c] = ( tk->dxyError() > 0. ? muonL2_dr[imu2c] / tk->dxyError() : 999. );

      // Dz (longitudinal distance to z=0 when at minimum transverse distance)
      // For baseline triggers, we do no cut (|dz|<9999 cm), neither at L2 nor at L3
      muonL2_dz[imu2c] = tk->dz(BSPosition);
      muonL2_vtxz[imu2c] = tk->dz();
      muonL2_nhits[imu2c] = tk->numberOfValidHits();
      muonL2_nchambers[imu2c] = validChambers(tk);
      muonL2_nstat[imu2c] = tk->hitPattern().muonStationsWithAnyHits();
      muonL2_ndtcscstat[imu2c] = tk->hitPattern().dtStationsWithAnyHits() + tk->hitPattern().cscStationsWithAnyHits();

      // At present we do not cut on this, but on a 90% CL value "ptLx" defined here below
      // We should change this in the future and cut directly on "pt", to avoid unnecessary complications and risks
      // Baseline cuts (HLT exercise):
      //                Relaxed Single muon:  ptLx>16 GeV
      //                Isolated Single muon: ptLx>11 GeV
      //                Relaxed Double muon: ptLx>3 GeV
      double l2_err0 = tk->error(0); // error on q/p
      double l2_abspar0 = fabs(tk->parameter(0)); // |q/p|
      //       double ptLx = tk->pt();
      // convert 50% efficiency threshold to 90% efficiency threshold
      // For L2 muons: nsigma_Pt_ = 3.9
      //       double nsigma_Pt_ = 3.9;
      // For L3 muons: nsigma_Pt_ = 2.2
      // these are the old TDR values for nsigma_Pt_
      // We know that these values are slightly smaller for CMSSW
      // But as quoted above, we want to get rid of this gymnastics in the future
      //       if (abspar0>0) ptLx += nsigma_Pt_*err0/abspar0*tk->pt();

      // Charge
      // We use the charge in some dimuon paths
      muonL2_pterr[imu2c] = l2_err0/l2_abspar0;
      muonL2_charge[imu2c] = tk->charge();

      l1t::MuonRef l1; 
      int il2 = 0; 
      //find the corresponding L1 
      l1 = tk->seedRef().castTo<edm::Ref< L2MuonTrajectorySeedCollection> >()->l1tParticle();
      il2++; 
      int imu1idx = 0; 
      if (MuCands1.isValid() && !MuCands1.failedToGet()) {
        for(int u = MuCands1->getFirstBX(); u <= MuCands1->getLastBX(); ++u) { 
          typedef std::vector<l1t::Muon>::const_iterator candl1;
          for (candl1 j=MuCands1->begin(u); j!=MuCands1->end(u); j++) { 
            if((j->pt() == l1->pt()) &&
               (j->eta() == l1->eta()) &&
               (j->phi() == l1->phi()) &&
               (j->hwQual() == l1->hwQual())
               )
              { break; }
            //	  std::cout << << std::endl;
            //          if ( tkl1 == l1 ) {break;} 
            imu1idx++; 
          } 
        } 
      }
      else {imu1idx = -999;} 
      muonL2_L1idx[imu2c] = imu1idx; // Index of the L1 muon having matched with the L2 muon with index imu2c 

      Double_t deta = ( tk->eta()-l1->eta() );
      Double_t dphi = TVector2::Phi_mpi_pi( tk->phi()-l1->phi());
      muonL2_L1dr[imu2c] = TMath::Sqrt( deta*deta+dphi*dphi );


      int imu2c2nd = imu2c + 1;// This will be the index in the hltTree for the 2nd muon of the dimuon combination

      for (cand j=i; j!=myMucands2.end(); j++) if (i!=j) {//Loop over all L2 muons from the one we are already treating
	reco::TrackRef tk2nd = j->get<reco::TrackRef>();

        ULong64_t trigBits2nd=0;
        for (unsigned int j2nd=0; j2nd<MuCands2FilterCollection.size(); j2nd++) {
          edm::Handle<reco::RecoChargedCandidateCollection>  MuCands2Tmp2nd = MuCands2FilterCollection.at(j2nd);
          if (MuCands2Tmp2nd.isValid() && !MuCands2Tmp2nd.failedToGet()) {
            for (cand kMuL2_2nd=MuCands2Tmp2nd->begin(); kMuL2_2nd!=MuCands2Tmp2nd->end(); kMuL2_2nd++) {
              reco::TrackRef tkFiltered2nd = kMuL2_2nd->get<reco::TrackRef>();
              if ( (tkFiltered2nd==tk2nd)  
                   ) {
                trigBits2nd += (ULong64_t) pow(2 ,j2nd);
                break;
              }
            }
          }
        }
        dimuonL2_trig[idimu2c] = trigBits & trigBits2nd;   

        dimuonL2_Mu1idx[idimu2c] = imu2c;
        dimuonL2_Mu2idx[idimu2c] = imu2c2nd;
        dimuonL2_charge[idimu2c] = tk->charge() + tk2nd->charge();
      
        double acop = fabs(tk->phi()-tk2nd->phi());
        if (acop>M_PI) acop = 2*M_PI - acop;
        acop = M_PI - acop;
        dimuonL2_dPhi[idimu2c] = acop;

        double angle = acos((tk->px()*tk2nd->px() + tk->py()*tk2nd->py() + tk->pz()*tk2nd->pz())/(tk->p()*tk2nd->p()));
        dimuonL2_angle[idimu2c] = angle;

        double ptbalance = fabs(tk->pt()-tk2nd->pt());
        dimuonL2_dPt[idimu2c] = ptbalance;

        // Combined dimuon system
        double const MuMass = 0.106;
        double const MuMass2 = MuMass*MuMass;
        double e1 = sqrt(tk->momentum().Mag2()+MuMass2);
        double e2 = sqrt(tk2nd->momentum().Mag2()+MuMass2);
        reco::Particle::LorentzVector p1 = reco::Particle::LorentzVector(tk->px(),tk->py(),tk->pz(),e1);
        reco::Particle::LorentzVector p2 = reco::Particle::LorentzVector(tk2nd->px(),tk2nd->py(),tk2nd->pz(),e2);
        reco::Particle::LorentzVector p = p1+p2;

        dimuonL2_invmass[idimu2c] = p.mass();
        dimuonL2_pt[idimu2c] = p.pt();
        dimuonL2_rap[idimu2c] = p.Rapidity();

	imu2c2nd++;
        idimu2c++;
      }

      imu2c++;
    }
    nDiMuonL2 = idimu2c;
  }
  else {nmuonL2 = 0; nDiMuonL2 = 0;}

  // Dealing with L3 muons
  reco::RecoChargedCandidateCollection myMucands3;
  if (MuCands3.isValid() && !MuCands3.failedToGet()) {
    int k = 0; 
    myMucands3 = * MuCands3;
    std::sort(myMucands3.begin(),myMucands3.end(),PtGreater());
    nmuonL3 = myMucands3.size();
    typedef reco::RecoChargedCandidateCollection::const_iterator cand;
    int imu3c=0;
    int idimu3c=0;
    for (cand i=myMucands3.begin(); i!=myMucands3.end(); i++) {
      reco::TrackRef tk = i->get<reco::TrackRef>();
      reco::RecoChargedCandidateRef candref = reco::RecoChargedCandidateRef(MuCands3,k);

      ULong64_t trigBits=0;
      for (unsigned int j=0; j<MuCands3FilterCollection.size(); j++) {
        edm::Handle<reco::RecoChargedCandidateCollection>  MuCands3Tmp = MuCands3FilterCollection.at(j);
        if (MuCands3Tmp.isValid() && !MuCands3Tmp.failedToGet()) {
          int kkk = 0; 
          for (cand kMuL3=MuCands3Tmp->begin(); kMuL3!=MuCands3Tmp->end(); kMuL3++) {
            reco::TrackRef tkFiltered = kMuL3->get<reco::TrackRef>();
            reco::RecoChargedCandidateRef candrefFiltered = reco::RecoChargedCandidateRef(MuCands3Tmp,kkk);
            if ( (tkFiltered==tk) && 
                 (candrefFiltered==candref)  
                 ) {
              trigBits += (ULong64_t) pow(2 ,j);
              break;
            }
            kkk++;
          }
        }
      }      
      muonL3_trig[imu3c] = trigBits;

      reco::TrackRef staTrack;
      typedef reco::MuonTrackLinksCollection::const_iterator l3muon;
      int il3 = 0;
      //find the corresponding L2 track
      staTrack = tk->seedRef().castTo<edm::Ref< L3MuonTrajectorySeedCollection> >()->l2Track();
      il3++;
      int imu2idx = 0;
      if (MuCands2.isValid() && !MuCands2.failedToGet()) {
	typedef reco::RecoChargedCandidateCollection::const_iterator candl2;
	for (candl2 i2=myMucands2.begin(); i2!=myMucands2.end(); i2++) {
	  reco::TrackRef tkl2 = i2->get<reco::TrackRef>();
	  if ( tkl2 == staTrack ) {break;}
	  imu2idx++;
	}
      }
      else {imu2idx = -999;}
      muonL3_global2idx[imu3c] = imu2idx; // Index of the L2 muon having matched with the L3 muon with index imu3c
      muonL3_L2idx[imu3c] = imu2idx;
      
      muonL3_L1dr[imu3c] = 0.0;
      muonL3_L2dr[imu3c] = 0.0;
      muonL3_TrackL2dr[imu3c] = 0.0;
      if (imu2idx>-1) {
        Double_t deta = ( staTrack->eta()-tk->eta() );
        Double_t dphi = TVector2::Phi_mpi_pi( staTrack->phi()-tk->phi());
        muonL3_L2dr[imu3c] = TMath::Sqrt( deta*deta+dphi*dphi );

        l1t::MuonRef l1; 
        l1 = staTrack->seedRef().castTo<edm::Ref< L2MuonTrajectorySeedCollection> >()->l1tParticle();
        deta = ( tk->eta()-l1->eta() );
        dphi = TVector2::Phi_mpi_pi( tk->phi()-l1->phi());
        muonL3_L1dr[imu3c] = TMath::Sqrt( deta*deta+dphi*dphi );
      }
        

      muonL3_globalpt[imu3c] = tk->pt();
      muonL3_pt[imu3c] = candref->pt();
      // eta (we require |eta|<2.5 in all filters
      muonL3_globaleta[imu3c] = tk->eta();
      muonL3_globalphi[imu3c] = tk->phi();
      muonL3_eta[imu3c] = candref->eta();
      muonL3_phi[imu3c] = candref->phi();


      //       // Dr (transverse distance to (0,0,0))
      //       // For baseline triggers, we do no cut at L2 (|dr|<9999 cm)
      //       // However, we use |dr|<300 microns at L3, which it probably too tough for LHC startup
      muonL3_dr[imu3c] = fabs( (- (candref->vx()-BSPosition.x()) * candref->py() + (candref->vy()-BSPosition.y()) * candref->px() ) / candref->pt() );
      muonL3_globalDxy[imu3c] = fabs(tk->dxy(BSPosition));
      muonL3_globalDxySig[imu3c] = ( tk->dxyError() > 0. ? muonL3_globalDxy[imu3c] / tk->dxyError() : -999. );

      //       // Dz (longitudinal distance to z=0 when at minimum transverse distance)
      //       // For baseline triggers, we do no cut (|dz|<9999 cm), neither at L2 nor at L3
      muonL3_dz[imu3c] = (candref->vz()-BSPosition.z()) - ((candref->vx()-BSPosition.x())*candref->px()+(candref->vy()-BSPosition.y())*candref->py())/candref->pt() * candref->pz()/candref->pt();
      muonL3_globaldz[imu3c] = tk->dz(BSPosition);

      muonL3_vtxz[imu3c] = candref->vz() - ( candref->vx()*candref->px() + candref->vy()*candref->py() )/candref->pt() * candref->pz()/candref->pt();
      muonL3_globalvtxz[imu3c] = tk->dz();
      //muonl3vtxz[imu3c] = candref->vz();
      //muonl3globalvtxz[imu3c] = tk->vz();

      muonL3_nhits[imu3c] = tk->numberOfValidHits();  

      //       // At present we do not cut on this, but on a 90% CL value "ptLx" defined here below
      //       // We should change this in the future and cut directly on "pt", to avoid unnecessary complications and risks
      //       // Baseline cuts (HLT exercise):
      //       //                Relaxed Single muon:  ptLx>16 GeV
      //       //                Isolated Single muon: ptLx>11 GeV
      //       //                Relaxed Double muon: ptLx>3 GeV
      double l3_err0 = tk->error(0); // error on q/p
      double l3_abspar0 = fabs(tk->parameter(0)); // |q/p|
      // //       double ptLx = tk->pt();
      //       // convert 50% efficiency threshold to 90% efficiency threshold
      //       // For L2 muons: nsigma_Pt_ = 3.9
      //       // For L3 muons: nsigma_Pt_ = 2.2
      // //       double nsigma_Pt_ = 2.2;
      //       // these are the old TDR values for nsigma_Pt_
      //       // We know that these values are slightly smaller for CMSSW
      //       // But as quoted above, we want to get rid of this gymnastics in the future
      // //       if (abspar0>0) ptLx += nsigma_Pt_*err0/abspar0*tk->pt();

      // Charge
      // We use the charge in some dimuon paths
      muonL3_pterr[imu3c] = l3_err0/l3_abspar0;
      muonL3_globalchg[imu3c] = tk->charge();
      muonL3_charge[imu3c] = candref->charge();

      muonL3_normchi2[imu3c] = tk->normalizedChi2();
      muonL3_npixelhits[imu3c] = tk->hitPattern().numberOfValidPixelHits();
      muonL3_ntrackerhits[imu3c] = tk->hitPattern().numberOfValidTrackerHits();
      muonL3_nmuonhits[imu3c] = tk->hitPattern().numberOfValidMuonHits();

      muonL3_ptLx[imu3c] = ( l3_abspar0>0 ? muonL3_pt[imu3c]*(1.0 + muonL3_pterr[imu3c]) : -999. );

      int imu3c2nd = imu3c + 1;// This will be the index in the hltTree for the 2nd muon of the dimuon combination

      for (cand j=i; j!=myMucands3.end(); j++) if (i!=j) {//Loop over all L3 muons from the one we are already treating
	reco::TrackRef tk2nd = j->get<reco::TrackRef>();
        reco::RecoChargedCandidateRef candref2nd = reco::RecoChargedCandidateRef(MuCands3,imu3c2nd);

        ULong64_t trigBits2nd=0;
        for (unsigned int j2nd=0; j2nd<MuCands3FilterCollection.size(); j2nd++) {
          edm::Handle<reco::RecoChargedCandidateCollection>  MuCands3Tmp2nd = MuCands3FilterCollection.at(j2nd);
          if (MuCands3Tmp2nd.isValid() && !MuCands3Tmp2nd.failedToGet()) {
            int kkk2nd = 0; 
            for (cand kMuL3_2nd=MuCands3Tmp2nd->begin(); kMuL3_2nd!=MuCands3Tmp2nd->end(); kMuL3_2nd++) {
              reco::TrackRef tkFiltered2nd = kMuL3_2nd->get<reco::TrackRef>();
              reco::RecoChargedCandidateRef candrefFiltered2nd = reco::RecoChargedCandidateRef(MuCands3Tmp2nd,kkk2nd);
              if ( (tkFiltered2nd==tk2nd) && 
                   (candrefFiltered2nd==candref2nd)  
                   ) {
                trigBits2nd += (ULong64_t) pow(2 ,j2nd);
                break;
              }
              kkk2nd++;
            }
          }
        }   
        dimuonL3_trig[idimu3c] = trigBits & trigBits2nd;   

        dimuonL3_Mu1idx[idimu3c] = imu3c;
        dimuonL3_Mu2idx[idimu3c] = imu3c2nd;
        dimuonL3_charge[idimu3c] = candref->charge() + candref2nd->charge();
      
        dimuonL3_dca[idimu3c] = -999.;
	reco::TransientTrack transMu1(*tk, &(*theMagField) );
	reco::TransientTrack transMu2(*tk2nd, &(*theMagField) );
	TrajectoryStateClosestToPoint mu1TS = transMu1.impactPointTSCP();
	TrajectoryStateClosestToPoint mu2TS = transMu2.impactPointTSCP();
	if (mu1TS.isValid() && mu2TS.isValid()) {
	  ClosestApproachInRPhi cApp;
	  cApp.calculate(mu1TS.theState(), mu2TS.theState());
	  if (cApp.status()) {
            dimuonL3_dca[idimu3c] = cApp.distance();
          }
        }

        double acop = fabs(candref->phi()-candref2nd->phi());
        if (acop>M_PI) acop = 2*M_PI - acop;
        acop = M_PI - acop;
        dimuonL3_dPhi[idimu3c] = acop;

        double ptbalance = fabs(candref->pt()-candref2nd->pt());
        dimuonL3_dPt[idimu3c] = ptbalance;

        bool isCowboy = (candref->charge()*acop > 0.);
        dimuonL3_isCowboy[idimu3c] = isCowboy;

        // Combined dimuon system
        double const MuMass = 0.106;
        double const MuMass2 = MuMass*MuMass;
        double e1 = sqrt(candref->momentum().Mag2()+MuMass2);
        double e2 = sqrt(candref2nd->momentum().Mag2()+MuMass2);
        reco::Particle::LorentzVector p1 = reco::Particle::LorentzVector(candref->px(),candref->py(),candref->pz(),e1);
        reco::Particle::LorentzVector p2 = reco::Particle::LorentzVector(candref2nd->px(),candref2nd->py(),candref2nd->pz(),e2);
        reco::Particle::LorentzVector p = p1+p2;

        dimuonL3_invmass[idimu3c] = p.mass();
        dimuonL3_pt[idimu3c] = p.pt();
        dimuonL3_rap[idimu3c] = p.Rapidity();

	imu3c2nd++;
        idimu3c++;
      }

      imu3c++;
      k++; 
    }
    nDiMuonL3 = idimu3c;
  }
  else {nmuonL3 = 0;  nDiMuonL3 = 0;}

  // Dealing with L3 Tracker Tracks for L2IOHits
  std::vector< reco::Track > myL3TkTracksFromL2OIHit;
  if (L3TkTracksFromL2OIHit.isValid() && !L3TkTracksFromL2OIHit.failedToGet()) {
    myL3TkTracksFromL2OIHit = * L3TkTracksFromL2OIHit;
    std::sort(myL3TkTracksFromL2OIHit.begin(),myL3TkTracksFromL2OIHit.end(),PtGreater());
    nL3TkL2OIHit = myL3TkTracksFromL2OIHit.size();
    typedef std::vector<reco::Track>::const_iterator cand;
    int iL3TkL2OIHit=0;
    for (cand i=myL3TkTracksFromL2OIHit.begin(); i!=myL3TkTracksFromL2OIHit.end(); i++) {
      reco::Track tk = *i;

      L3TkL2OIHit_pt[iL3TkL2OIHit] = tk.pt();
      L3TkL2OIHit_eta[iL3TkL2OIHit] = tk.eta();
      L3TkL2OIHit_phi[iL3TkL2OIHit] = tk.phi();

      bool canUseL3MTS = false;
      // check the seedRef is non-null first; and then
      if (tk.seedRef().isNonnull()){
        const L3MuonTrajectorySeed* a = dynamic_cast<const L3MuonTrajectorySeed*>(tk.seedRef().get());
        canUseL3MTS = a != nullptr;
      }
      if (canUseL3MTS){
    	edm::Ref<L3MuonTrajectorySeedCollection> l3seedRef = tk.seedRef().castTo<edm::Ref<L3MuonTrajectorySeedCollection> >() ;
    	reco::TrackRef staTrack = l3seedRef->l2Track();
        int imu3idx = 0;
        if (MuCands3.isValid() && !MuCands3.failedToGet()) {
          typedef reco::RecoChargedCandidateCollection::const_iterator candl3;
          for (candl3 i3=myMucands3.begin(); i3!=myMucands3.end(); i3++) {
            reco::TrackRef tkl3 = i3->get<reco::TrackRef>();
            reco::TrackRef staTrackL3 = tkl3->seedRef().castTo<edm::Ref< L3MuonTrajectorySeedCollection> >()->l2Track();
            if ( staTrack == staTrackL3 ) {break;}
            imu3idx++;
          }
        }
        else {imu3idx = -999;}        
        L3TkL2OIHit_L3idx[iL3TkL2OIHit] = imu3idx;
        
        int imu2idx = 0;
        if (MuCands2.isValid() && !MuCands2.failedToGet()) {
          typedef reco::RecoChargedCandidateCollection::const_iterator candl2;
          for (candl2 i2=myMucands2.begin(); i2!=myMucands2.end(); i2++) {
            reco::TrackRef tkl2 = i2->get<reco::TrackRef>();
            if ( tkl2 == staTrack ) {break;}
            imu2idx++;
          }
        }
        else {imu2idx = -999;}
        L3TkL2OIHit_L2idx[iL3TkL2OIHit] = imu2idx;
      }
      else { 
        L3TkL2OIHit_L3idx[iL3TkL2OIHit] = -999; 
        L3TkL2OIHit_L2idx[iL3TkL2OIHit] = -999;
      }

      L3TkL2OIHit_dr[iL3TkL2OIHit] = fabs(tk.dxy(BSPosition));
      L3TkL2OIHit_drError[iL3TkL2OIHit] = tk.dxyError();
      L3TkL2OIHit_dz[iL3TkL2OIHit] = fabs(tk.dz(BSPosition));
        
      L3TkL2OIHit_normchi2[iL3TkL2OIHit] = tk.normalizedChi2();
      L3TkL2OIHit_npixelhits[iL3TkL2OIHit] = tk.hitPattern().numberOfValidPixelHits();
      L3TkL2OIHit_ntrackerhits[iL3TkL2OIHit] = tk.hitPattern().numberOfValidTrackerHits();

      L3TkL2OIHit_nhits[iL3TkL2OIHit] = tk.numberOfValidHits();  

      L3TkL2OIHit_charge[iL3TkL2OIHit] = tk.charge();

      iL3TkL2OIHit++;
    }
  }
  else {nL3TkL2OIHit = 0;}


  // Dealing with L3 Tracker Tracks for L2IOStates
  std::vector< reco::Track > myL3TkTracksFromL2OIState;
  if (L3TkTracksFromL2OIState.isValid() && !L3TkTracksFromL2OIState.failedToGet()) {
    myL3TkTracksFromL2OIState = * L3TkTracksFromL2OIState;
    std::sort(myL3TkTracksFromL2OIState.begin(),myL3TkTracksFromL2OIState.end(),PtGreater());
    nL3TkL2OIState = myL3TkTracksFromL2OIState.size();
    typedef std::vector<reco::Track>::const_iterator cand;
    int iL3TkL2OIState=0;
    for (cand i=myL3TkTracksFromL2OIState.begin(); i!=myL3TkTracksFromL2OIState.end(); i++) {
      reco::Track tk = *i;

      L3TkL2OIState_pt[iL3TkL2OIState] = tk.pt();
      L3TkL2OIState_eta[iL3TkL2OIState] = tk.eta();
      L3TkL2OIState_phi[iL3TkL2OIState] = tk.phi();

      bool canUseL3MTS = false;
      // check the seedRef is non-null first; and then
      if (tk.seedRef().isNonnull()){
        const L3MuonTrajectorySeed* a = dynamic_cast<const L3MuonTrajectorySeed*>(tk.seedRef().get());
        canUseL3MTS = a != nullptr;
      }
      if (canUseL3MTS){
    	edm::Ref<L3MuonTrajectorySeedCollection> l3seedRef = tk.seedRef().castTo<edm::Ref<L3MuonTrajectorySeedCollection> >() ;
    	reco::TrackRef staTrack = l3seedRef->l2Track();
        int imu3idx = 0;
        if (MuCands3.isValid() && !MuCands3.failedToGet()) {
          typedef reco::RecoChargedCandidateCollection::const_iterator candl3;
          for (candl3 i3=myMucands3.begin(); i3!=myMucands3.end(); i3++) {
            reco::TrackRef tkl3 = i3->get<reco::TrackRef>();
            reco::TrackRef staTrackL3 = tkl3->seedRef().castTo<edm::Ref< L3MuonTrajectorySeedCollection> >()->l2Track();
            if ( staTrack == staTrackL3 ) {break;}
            imu3idx++;
          }
        }
        else {imu3idx = -999;}        
        L3TkL2OIState_L3idx[iL3TkL2OIState] = imu3idx;
        
        int imu2idx = 0;
        if (MuCands2.isValid() && !MuCands2.failedToGet()) {
          typedef reco::RecoChargedCandidateCollection::const_iterator candl2;
          for (candl2 i2=myMucands2.begin(); i2!=myMucands2.end(); i2++) {
            reco::TrackRef tkl2 = i2->get<reco::TrackRef>();
            if ( tkl2 == staTrack ) {break;}
            imu2idx++;
          }
        }
        else {imu2idx = -999;}
        L3TkL2OIState_L2idx[iL3TkL2OIState] = imu2idx;
      }
      else { 
        L3TkL2OIState_L3idx[iL3TkL2OIState] = -999; 
        L3TkL2OIState_L2idx[iL3TkL2OIState] = -999;
      }

      L3TkL2OIState_dr[iL3TkL2OIState] = fabs(tk.dxy(BSPosition));
      L3TkL2OIState_drError[iL3TkL2OIState] = tk.dxyError();
      L3TkL2OIState_dz[iL3TkL2OIState] = fabs(tk.dz(BSPosition));
        
      L3TkL2OIState_normchi2[iL3TkL2OIState] = tk.normalizedChi2();
      L3TkL2OIState_npixelhits[iL3TkL2OIState] = tk.hitPattern().numberOfValidPixelHits();
      L3TkL2OIState_ntrackerhits[iL3TkL2OIState] = tk.hitPattern().numberOfValidTrackerHits();

      L3TkL2OIState_nhits[iL3TkL2OIState] = tk.numberOfValidHits();  

      L3TkL2OIState_charge[iL3TkL2OIState] = tk.charge();

      iL3TkL2OIState++;
    }
  }
  else {nL3TkL2OIState = 0;}
}

int HLTMuon::validChambers(const reco::TrackRef & track)
{
  // count hits in chambers using std::maps
  std::map<uint32_t,int> DTchambers;
  std::map<uint32_t,int> CSCchambers;

  for (trackingRecHit_iterator hit = track->recHitsBegin();  hit != track->recHitsEnd();  ++hit) {
    if( !((*hit)->isValid()) ) continue;

    DetId id = (*hit)->geographicalId();

    if (id.det() == DetId::Muon  &&  id.subdetId() == MuonSubdetId::DT) {
      // get the DT chamber index, not the layer index, by using DTChamberId
      uint32_t index = DTChamberId(id).rawId();

      if (DTchambers.find(index) == DTchambers.end()) {
        DTchambers[index] = 0;
      }
      DTchambers[index]++;
    }

    else if (id.det() == DetId::Muon  &&  id.subdetId() == MuonSubdetId::CSC) {
      // get the CSC chamber index, not the layer index, by explicitly setting the layer id to 0
      CSCDetId id2(id);
      uint32_t index = CSCDetId(id2.endcap(), id2.station(), id2.ring(), id2.chamber(), 0);

      if (CSCchambers.find(index) == CSCchambers.end()) {
        CSCchambers[index] = 0;
      }
      CSCchambers[index]++;
    }
  }

  // count chambers that satisfy minimal numbers of hits per chamber
  int validChambers = 0;

  int minDThits = 1;
  int minCSChits = 1;

  for (std::map<uint32_t,int>::const_iterator iter = DTchambers.begin();  iter != DTchambers.end();  ++iter) {
    if (iter->second >= minDThits) {
      validChambers++;
    }
  }
  for (std::map<uint32_t,int>::const_iterator iter = CSCchambers.begin();  iter != CSCchambers.end();  ++iter) {
    if (iter->second >= minCSChits) {
      validChambers++;
    }
  }
  return validChambers;
}
