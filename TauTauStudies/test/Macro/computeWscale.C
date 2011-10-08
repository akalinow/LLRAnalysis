


{

  TFile *f = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root","READ");

  TTree* tree = f->Get("outTreePtOrd");
  float MtLeg1; int tightestHPWP;
  tree->SetBranchAddress("MtLeg1",&MtLeg1);
  tree->SetBranchAddress("tightestHPSWP",&tightestHPWP);

  TBranch *brMtLeg1 = tree->GetBranch("MtLeg1");
  TBranch *brtightestHPWP = tree->GetBranch("tightestHPSWP");

  int low   = 0;
  int lowH  = 0;
  int lowL  = 0;
  int high  = 0;
  int highH = 0;
  int highL = 0;

  int nEntries = tree->GetEntries();
  for(unsigned n = 0 ; n< nEntries ; n++){
    brMtLeg1->GetEntry(n);
    brtightestHPWP->GetEntry(n);

    if(tightestHPWP<1) continue;

    if(MtLeg1>60){
      high++;
    }
    if(MtLeg1*1.01>60){
      highH++;
    }
    if(MtLeg1*0.99>60){
      highL++;
    }
    if(MtLeg1<40){
      low++;
    }
    if(MtLeg1*1.01<40){
      lowH++;
    }
    if(MtLeg1*0.99<40){
      lowL++;
    }
  }


  cout << low << " -- " << lowH << " --" << lowL << endl;
  cout << high << " -- " << highH << " --" << highL << endl;


}
