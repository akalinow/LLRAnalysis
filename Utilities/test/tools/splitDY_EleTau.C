{

  TString dirOut="./";

  TString tree="outTreePtOrd";
  const int nAn=5;
  TString analysis[nAn] = {"","TauUp","TauDown","JetUp","JetDown"};
  TString suffix[nAn]   = {"nominal","TauUp","TauDown","JetUp","JetDown"};

  TFile *fBackgroundDY[nAn];  
  TFile *fBackgroundDY_TauTau[nAn];  
  TFile *fBackgroundDY_EToTau[nAn];  
  TFile *fBackgroundDY_JetToTau[nAn];  

  for(int iAn=0 ; iAn<nAn ; iAn++) {
    fBackgroundDY[iAn]          = new TFile(dirOut+"/nTupleDYJets_ElecTau_"+suffix[iAn]+".root","READ");
    fBackgroundDY_TauTau[iAn]   = new TFile(dirOut+"/nTupleDYJetsTauTau_ElecTau_"+suffix[iAn]+".root","UPDATE");
    fBackgroundDY_EToTau[iAn]  = new TFile(dirOut+"/nTupleDYJetsEToTau_ElecTau_"+suffix[iAn]+".root","UPDATE");
    fBackgroundDY_JetToTau[iAn] = new TFile(dirOut+"/nTupleDYJetsJetToTau_ElecTau_"+suffix[iAn]+".root","UPDATE");
  }

  for(int iAn=0 ; iAn<nAn ; iAn++) {
    fBackgroundDY_TauTau[iAn]->cd();
    TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY[iAn]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)==(23*15)");
    backgroundDYTauTau->Write();

    fBackgroundDY_EToTau[iAn]->cd();
    TTree *backgroundDYEToTau  = ((TTree*)fBackgroundDY[iAn]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau");
    backgroundDYEToTau->Write();

    fBackgroundDY_JetToTau[iAn]->cd();
    TTree *backgroundDYJetToTau  = ((TTree*)fBackgroundDY[iAn]->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  !leptFakeTau");
    backgroundDYJetToTau->Write();
  }

  for(int iAn=0 ; iAn<nAn ; iAn++) {
    fBackgroundDY[iAn]          ->Close();
    fBackgroundDY_TauTau[iAn]   ->Close();
    fBackgroundDY_EToTau[iAn]  ->Close();
    fBackgroundDY_JetToTau[iAn] ->Close();
  }

  //delete fBackgroundDY;

}
