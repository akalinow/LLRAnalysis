{

  //TTree *backgroundDYTauTau, *backgroundDYMutoTau, *backgroundDYJtoTau;
  TString tree="outTreePtOrd";
  //TString dirOut="/data_CMS/cms/htautau/SoftLeptonAnalysis/MuTauMet/round4/";
  TString dirOut="";

  TFile *fBackgroundDY    = new TFile(dirOut+"/.root","READ");
  
  //TFile* file_TauTau   = new TFile(dirOut+"/nTuple_DYJ_TauTau_Open_MuTauStream.root","RECREATE");
  //TFile* file_MuToTau  = new TFile(dirOut+"/nTuple_DYJ_MuToTau_Open_MuTauStream.root","RECREATE");
  //TFile* file_JetToTau = new TFile(dirOut+"/nTuple_DYJ_JetToTau_Open_MuTauStream.root","RECREATE");

  const int nAn=5;
  TString analysis[nAn] = {"","TauUp","TauDown","JetUp","JetDown"};

  //TFile* file1 = TFile::Open(dirOut+"/nTuple_DYJ_TauTau_Open_MuTauStream.root","UPDATE");
  TFile* file2 = TFile::Open(dirOut+"/nTuple_DYJ_MuToTau_Open_MuTauStream.root","UPDATE");  
  TFile* file3 = TFile::Open(dirOut+"/nTuple_DYJ_JetToTau_Open_MuTauStream.root","UPDATE");

  /*
  for(int iAn=0 ; iAn<nAn ; iAn++) {
//     file1->cd();
//     TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)==(23*15)");
//     backgroundDYTauTau->Write();
    //file1->Close();
    //delete backgroundDYTauTau;
    //delete file;
  }
  */
  for(int iAn=0 ; iAn<nAn ; iAn++) {
    file2->cd();
    TTree *backgroundDYMuToTau  = ((TTree*)fBackgroundDY->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau");
    backgroundDYMuToTau->Write();
    //file->Close();
    //delete backgroundDYMuToTau;
    //delete file;
  }
  for(int iAn=0 ; iAn<nAn ; iAn++) {
    file3->cd();
    TTree *backgroundDYJetToTau  = ((TTree*)fBackgroundDY->Get(tree+analysis[iAn]))->CopyTree("abs(genDecay)!=(23*15) &&  !leptFakeTau");
    backgroundDYJetToTau->Write();
    //file->Close();
    //delete backgroundDYJetToTau;
    //delete file;
  }

  fBackgroundDY->Close(); 
  //file1->Close(); 
  file2->Close(); file3->Close();
  //delete fBackgroundDY;

}
