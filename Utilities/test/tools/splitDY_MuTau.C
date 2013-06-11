{

  TString dirOut="./";

  TString tree="outTreePtOrd";
  const int nAn=5;
  TString analysis[nAn] = {"","TauUp","TauDown","JetUp","JetDown"};
  TString suffix[nAn]   = {"nominal","TauUp","TauDown","JetUp","JetDown"};

  /*
  const int nType=5;
  TString type[nType]={"DYJets","DYJets1Jets","DYJets2Jets","DYJets3Jets","DYJets4Jets"};
  */
  const int nType=2;
  TString type[nType]={"DYJets","DYJets1Jets"};

  TFile *fBackgroundDY[nAn][nType];  
  TFile *fBackgroundDY_TauTau[nAn][nType];  
  TFile *fBackgroundDY_MuToTau[nAn][nType];  
  TFile *fBackgroundDY_JetToTau[nAn][nType];  

  for(int iType=0; iType<nType ; iType++) {
    for(int iAn=0 ; iAn<nAn ; iAn++) {
      fBackgroundDY[iAn][iType]          = new TFile(dirOut+"/nTuple"+type[iType]+"_MuTau_"+suffix[iAn]+".root","READ");
      fBackgroundDY_TauTau[iAn][iType]   = new TFile(dirOut+"/nTuple"+type[iType]+"TauTau_MuTau_"+suffix[iAn]+".root","UPDATE");
      fBackgroundDY_MuToTau[iAn][iType]  = new TFile(dirOut+"/nTuple"+type[iType]+"MuToTau_MuTau_"+suffix[iAn]+".root","UPDATE");
      fBackgroundDY_JetToTau[iAn][iType] = new TFile(dirOut+"/nTuple"+type[iType]+"JetToTau_MuTau_"+suffix[iAn]+".root","UPDATE");
    }
  }

  for(int iType=0; iType<nType ; iType++) {
    for(int iAn=0 ; iAn<nAn ; iAn++) {
      fBackgroundDY_TauTau[iAn][iType]->cd();
      TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY[iAn][iType]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)==(23*15)");
      backgroundDYTauTau->Write();

      fBackgroundDY_MuToTau[iAn][iType]->cd();
      TTree *backgroundDYMuToTau  = ((TTree*)fBackgroundDY[iAn][iType]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau");
      backgroundDYMuToTau->Write();

      fBackgroundDY_JetToTau[iAn][iType]->cd();
      TTree *backgroundDYJetToTau  = ((TTree*)fBackgroundDY[iAn][iType]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  !leptFakeTau");
      backgroundDYJetToTau->Write();
    }
  }

  for(int iType=0; iType<nType ; iType++) {
    for(int iAn=0 ; iAn<nAn ; iAn++) {
      fBackgroundDY[iAn][iType]          ->Close();
      fBackgroundDY_TauTau[iAn][iType]   ->Close();
      fBackgroundDY_MuToTau[iAn][iType]  ->Close();
      fBackgroundDY_JetToTau[iAn][iType] ->Close();
    }
  }
  //delete fBackgroundDY;

}
