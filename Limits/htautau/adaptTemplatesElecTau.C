



{

  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// ADAPT THE TEMPLATES FOR ELEC+TAU  ///////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  //TString inputDir = "datacards";
  TString inputDir = ".";  

  TString channel = "ele";
  //TString variable = "diTauVisMass";
  TString variable = "diTauNSVfitMass";

  bool onlyJetMorphingZ  = true;
  bool fixJetScaleForW   = true;
  bool fixJetScaleForVV  = true;
  bool fixJetScaleForTT  = true;
  bool fixJetScaleForZTT = true;
  bool fixJetScaleForZL  = true;
  bool fixJetScaleForZJ  = true;
  bool fixJetScaleForSM  = true;

  float jetScaleForW     = 0.20;
  float jetScaleForVV    = 0.10;
  float jetScaleForTT    = 0.04;
  float jetScaleForZJ    = 0.20;
  float jetScaleForZL    = 0.20;
  float jetScaleForZTT   = 0.20;
  float jetScaleForSM    = 0.15;

  std::vector<TString> analysis;
  analysis.push_back(TString(""));
  analysis.push_back(TString("_CMS_scale_tUp"));
  analysis.push_back(TString("_CMS_scale_tDown"));
  analysis.push_back(TString("_CMS_scale_jUp"));
  analysis.push_back(TString("_CMS_scale_jDown"));
  analysis.push_back(TString("_CMS_scale_eUp"));
  analysis.push_back(TString("_CMS_scale_eDown"));

  TFile fin(inputDir+"/"+channel+"TauSM_"+variable+".root","UPDATE");

  // this loop changes the shapes to those of a different selection
  // but keeps the normalizations unchanged
  for(int k = 0 ; k < analysis.size() ; k++){

    fin.cd(channel+"Tau_SM0");
    TH1F* hQCD_new = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hW_new   = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hTT_new  = (TH1F*)gDirectory->Get("TT"+analysis[k]);
    TH1F* hVV_new  = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    TH1F* hZL_new  = (TH1F*)gDirectory->Get("ZL"+analysis[k]);
    TH1F* hZJ_new  = (TH1F*)gDirectory->Get("ZJ"+analysis[k]);
    
    fin.cd(channel+"Tau_SM2");
    TH1F* hQCD_old = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hW_old   = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hZTT_old = (TH1F*)gDirectory->Get("ZTT"+analysis[k]);
    TH1F* hTT_old  = (TH1F*)gDirectory->Get("TT"+analysis[k]);
    TH1F* hVV_old  = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    TH1F* hZL_old  = (TH1F*)gDirectory->Get("ZL"+analysis[k]);
    TH1F* hZJ_old  = (TH1F*)gDirectory->Get("ZJ"+analysis[k]);
    TH1F* hSM105_old    = (TH1F*)gDirectory->Get("SM105"+analysis[k]);
    TH1F* hSM110_old    = (TH1F*)gDirectory->Get("SM110"+analysis[k]);
    TH1F* hSM115_old    = (TH1F*)gDirectory->Get("SM115"+analysis[k]);
    TH1F* hSM120_old    = (TH1F*)gDirectory->Get("SM120"+analysis[k]);
    TH1F* hSM125_old    = (TH1F*)gDirectory->Get("SM125"+analysis[k]);
    TH1F* hSM130_old    = (TH1F*)gDirectory->Get("SM130"+analysis[k]);
    TH1F* hSM135_old    = (TH1F*)gDirectory->Get("SM135"+analysis[k]);
    TH1F* hSM140_old    = (TH1F*)gDirectory->Get("SM140"+analysis[k]);


    fin.cd(channel+"Tau_SMpre2");
    TH1F* hQCD_help   = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hZTT_help   = (TH1F*)gDirectory->Get("ZTT"+analysis[k]);
    TH1F* hW_help     = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hTT_help    = (TH1F*)gDirectory->Get("TT"+analysis[k]);
    TH1F* hVV_help    = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    TH1F* hSM105_help    = (TH1F*)gDirectory->Get("SM105"+analysis[k]);
    TH1F* hSM110_help    = (TH1F*)gDirectory->Get("SM110"+analysis[k]);
    TH1F* hSM115_help    = (TH1F*)gDirectory->Get("SM115"+analysis[k]);
    TH1F* hSM120_help    = (TH1F*)gDirectory->Get("SM120"+analysis[k]);
    TH1F* hSM125_help    = (TH1F*)gDirectory->Get("SM125"+analysis[k]);
    TH1F* hSM130_help    = (TH1F*)gDirectory->Get("SM130"+analysis[k]);
    TH1F* hSM135_help    = (TH1F*)gDirectory->Get("SM135"+analysis[k]);
    TH1F* hSM140_help    = (TH1F*)gDirectory->Get("SM140"+analysis[k]);

    
    fin.cd(channel+"Tau_SMpre2a");
    TH1F* hQCD_help2   = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hVV_help2    = (TH1F*)gDirectory->Get("VV"+analysis[k]);

    // directory to overwrite:
    fin.cd(channel+"Tau_SM2");

    ////////////////////////////////////////////////////////
    float QCD_old = hQCD_old->Integral();
    hQCD_old->Reset();
    hQCD_old->Add( hQCD_help2 , 1.0);
    hQCD_old->Scale(QCD_old/hQCD_old->Integral());
    hQCD_old->Write("QCD"+analysis[k] , TObject::kOverwrite);
    
    float ZTT_old = hZTT_old->Integral();
    hZTT_old->Reset();
    hZTT_old->Add(hZTT_help, ZTT_old/hZTT_help->Integral());
    hZTT_old->Write("ZTT"+analysis[k] , TObject::kOverwrite);

    float W_old = hW_old->Integral();
    hW_old->Reset();
    hW_old->Add(hW_help, W_old/hW_help->Integral());
    hW_old->Write("W"+analysis[k] , TObject::kOverwrite);

    float TT_old = hTT_old->Integral();
    hTT_old->Reset();
    hTT_old->Add(hTT_help, TT_old/hTT_help->Integral());
    hTT_old->Write("TT"+analysis[k] , TObject::kOverwrite);
    
    float VV_old = hVV_old->Integral();
    hVV_old->Reset();
    hVV_old->Add(hVV_help2, VV_old/hVV_help2->Integral());
    hVV_old->Write("VV"+analysis[k] , TObject::kOverwrite);
    
    float ZL_old = hZL_old->Integral();
    hZL_old->Reset();
    hZL_old->Add(hZL_new, ZL_old/hZL_new->Integral());
    hZL_old->Write("ZL"+analysis[k] , TObject::kOverwrite);
    
    float ZJ_old = hZJ_old->Integral();
    hZJ_old->Reset();
    hZJ_old->Add(hZJ_new, ZJ_old/hZJ_new->Integral());
    hZJ_old->Write("ZJ"+analysis[k] , TObject::kOverwrite);

    float SM105_old = hSM105_old->Integral();
    hSM105_old->Reset();
    hSM105_old->Add(hSM105_help, SM105_old/hSM105_help->Integral());
    hSM105_old->Write("SM105"+analysis[k] , TObject::kOverwrite);

    float SM110_old = hSM110_old->Integral();
    hSM110_old->Reset();
    hSM110_old->Add(hSM110_help, SM110_old/hSM110_help->Integral());
    //hSM110_old->Write("SM110"+analysis[k] , TObject::kOverwrite);

    float SM115_old = hSM115_old->Integral();
    hSM115_old->Reset();
    hSM115_old->Add(hSM115_help, SM115_old/hSM115_help->Integral());
    //hSM115_old->Write("SM115"+analysis[k] , TObject::kOverwrite);

    float SM120_old = hSM120_old->Integral();
    hSM120_old->Reset();
    hSM120_old->Add(hSM120_help, SM120_old/hSM120_help->Integral());
    //hSM120_old->Write("SM120"+analysis[k] , TObject::kOverwrite);

    float SM125_old = hSM125_old->Integral();
    hSM125_old->Reset();
    hSM125_old->Add(hSM125_help, SM125_old/hSM125_help->Integral());
    //hSM125_old->Write("SM125"+analysis[k] , TObject::kOverwrite);

    float SM130_old = hSM130_old->Integral();
    hSM130_old->Reset();
    hSM130_old->Add(hSM130_help, SM130_old/hSM130_help->Integral());
    //hSM130_old->Write("SM130"+analysis[k] , TObject::kOverwrite);

    float SM135_old = hSM135_old->Integral();
    hSM135_old->Reset();
    hSM135_old->Add(hSM135_help, SM135_old/hSM135_help->Integral());
    //hSM135_old->Write("SM135"+analysis[k] , TObject::kOverwrite);

    float SM140_old = hSM140_old->Integral();
    hSM140_old->Reset();
    hSM140_old->Add(hSM140_help, SM140_old/hSM140_help->Integral());
    //hSM140_old->Write("SM140"+analysis[k] , TObject::kOverwrite);

   
    ///////////////////////////////////////////////////////

  }


  // here we change the nornalization for the UP and DOWN templates
  // for those samples which have few events in the vbf region
  // the relative change in yiled is fixed to that measured in the 2-jet bin

  // fix the Z template
  cout << "Now fixing the Z templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  fin.cd(channel+"Tau_SM2");
  TH1F* hZTT                  = (TH1F*)gDirectory->Get("ZTT");
  TH1F* hZTT_CMS_scale_tUp    = (TH1F*)gDirectory->Get("ZTT_CMS_scale_tUp");
  TH1F* hZTT_CMS_scale_tDown  = (TH1F*)gDirectory->Get("ZTT_CMS_scale_tDown");
  TH1F* hZTT_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jUp");
  TH1F* hZTT_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jDown");
  TH1F* hZTT_CMS_scale_eUp    = (TH1F*)gDirectory->Get("ZTT_CMS_scale_eUp");
  TH1F* hZTT_CMS_scale_eDown  = (TH1F*)gDirectory->Get("ZTT_CMS_scale_eDown");

  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZTT_help                   = (TH1F*)gDirectory->Get("ZTT");
  TH1F* hZTT_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("ZTT_CMS_scale_tUp");
  TH1F* hZTT_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("ZTT_CMS_scale_tDown");
  float ratioUp   = (hZTT_CMS_scale_tUp_help->Integral()   - hZTT_help->Integral())/hZTT_help->Integral();
  float ratioDown = (hZTT_CMS_scale_tDown_help->Integral() - hZTT_help->Integral())/hZTT_help->Integral();
  cout << "scale_t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZTT_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hZTT->Integral() / hZTT_CMS_scale_tUp->Integral() );
  hZTT_CMS_scale_tDown->Scale( ( 1+ratioDown )*hZTT->Integral() / hZTT_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZTT_help                   = (TH1F*)gDirectory->Get("ZTT");
  TH1F* hZTT_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jUp");
  TH1F* hZTT_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jDown");
  float ratioUp   = (hZTT_CMS_scale_jUp_help->Integral()   - hZTT_help->Integral())/hZTT_help->Integral();
  float ratioDown = (hZTT_CMS_scale_jDown_help->Integral() - hZTT_help->Integral())/hZTT_help->Integral();
  if(fixJetScaleForZTT){
    ratioUp   =  jetScaleForZTT;
    ratioDown = -jetScaleForZTT;
  }
  cout << "scale_j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZTT_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hZTT->Integral() / hZTT_CMS_scale_jUp->Integral() );
  hZTT_CMS_scale_jDown->Scale( ( 1+ratioDown )*hZTT->Integral() / hZTT_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZTT_help                   = (TH1F*)gDirectory->Get("ZTT");
  TH1F* hZTT_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("ZTT_CMS_scale_eUp");
  TH1F* hZTT_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("ZTT_CMS_scale_eDown");
  float ratioUp   = (hZTT_CMS_scale_eUp_help->Integral()   - hZTT_help->Integral())/hZTT_help->Integral();
  float ratioDown = (hZTT_CMS_scale_eDown_help->Integral() - hZTT_help->Integral())/hZTT_help->Integral();
  cout << "scale_e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZTT_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hZTT->Integral() / hZTT_CMS_scale_eUp->Integral() );
  hZTT_CMS_scale_eDown->Scale( ( 1+ratioDown )*hZTT->Integral() / hZTT_CMS_scale_eDown->Integral() );

  fin.cd(channel+"Tau_SM2");
  hZTT_CMS_scale_tUp->Write("ZTT_CMS_scale_tUp" , TObject::kOverwrite);
  hZTT_CMS_scale_tDown->Write("ZTT_CMS_scale_tDown" , TObject::kOverwrite);
  hZTT_CMS_scale_jUp->Write("ZTT_CMS_scale_jUp" , TObject::kOverwrite);
  hZTT_CMS_scale_jDown->Write("ZTT_CMS_scale_jDown" , TObject::kOverwrite);
  hZTT_CMS_scale_eUp->Write("ZTT_CMS_scale_eUp" , TObject::kOverwrite);
  hZTT_CMS_scale_eDown->Write("ZTT_CMS_scale_eDown" , TObject::kOverwrite);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // fix the VV template
  cout << "Now fixing the VV templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  fin.cd(channel+"Tau_SM2");
  TH1F* hVV                  = (TH1F*)gDirectory->Get("VV");
  TH1F* hVV_CMS_scale_tUp    = (TH1F*)gDirectory->Get("VV_CMS_scale_tUp");
  TH1F* hVV_CMS_scale_tDown  = (TH1F*)gDirectory->Get("VV_CMS_scale_tDown");
  TH1F* hVV_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VV_CMS_scale_jUp");
  TH1F* hVV_CMS_scale_jDown  = (TH1F*)gDirectory->Get("VV_CMS_scale_jDown");
  TH1F* hVV_CMS_scale_eUp    = (TH1F*)gDirectory->Get("VV_CMS_scale_eUp");
  TH1F* hVV_CMS_scale_eDown  = (TH1F*)gDirectory->Get("VV_CMS_scale_eDown");

  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hVV_help                   = (TH1F*)gDirectory->Get("VV");
  TH1F* hVV_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("VV_CMS_scale_tUp");
  TH1F* hVV_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("VV_CMS_scale_tDown");
  float ratioUp   = (hVV_CMS_scale_tUp_help->Integral()   - hVV_help->Integral())/hVV_help->Integral();
  float ratioDown = (hVV_CMS_scale_tDown_help->Integral() - hVV_help->Integral())/hVV_help->Integral();
  cout << "scale_t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hVV_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hVV->Integral() / hVV_CMS_scale_tUp->Integral() );
  hVV_CMS_scale_tDown->Scale( ( 1+ratioDown )*hVV->Integral() / hVV_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hVV_help                   = (TH1F*)gDirectory->Get("VV");
  TH1F* hVV_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("VV_CMS_scale_jUp");
  TH1F* hVV_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("VV_CMS_scale_jDown");
  float ratioUp   = (hVV_CMS_scale_jUp_help->Integral()   - hVV_help->Integral())/hVV_help->Integral();
  float ratioDown = (hVV_CMS_scale_jDown_help->Integral() - hVV_help->Integral())/hVV_help->Integral();
  if(fixJetScaleForVV){
    ratioUp   =  jetScaleForVV;
    ratioDown = -jetScaleForVV;
  }
  cout << "scale_j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hVV_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hVV->Integral() / hVV_CMS_scale_jUp->Integral() );
  hVV_CMS_scale_jDown->Scale( ( 1+ratioDown )*hVV->Integral() / hVV_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hVV_help                   = (TH1F*)gDirectory->Get("VV");
  TH1F* hVV_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("VV_CMS_scale_eUp");
  TH1F* hVV_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("VV_CMS_scale_eDown");
  float ratioUp   = (hVV_CMS_scale_eUp_help->Integral()   - hVV_help->Integral())/hVV_help->Integral();
  float ratioDown = (hVV_CMS_scale_eDown_help->Integral() - hVV_help->Integral())/hVV_help->Integral();
  cout << "scale_e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hVV_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hVV->Integral() / hVV_CMS_scale_eUp->Integral() );
  hVV_CMS_scale_eDown->Scale( ( 1+ratioDown )*hVV->Integral() / hVV_CMS_scale_eDown->Integral() );

  fin.cd(channel+"Tau_SM2");
  hVV_CMS_scale_tUp->Write("VV_CMS_scale_tUp" , TObject::kOverwrite);
  hVV_CMS_scale_tDown->Write("VV_CMS_scale_tDown" , TObject::kOverwrite);
  hVV_CMS_scale_jUp->Write("VV_CMS_scale_jUp" , TObject::kOverwrite);
  hVV_CMS_scale_jDown->Write("VV_CMS_scale_jDown" , TObject::kOverwrite);
  hVV_CMS_scale_eUp->Write("VV_CMS_scale_eUp" , TObject::kOverwrite);
  hVV_CMS_scale_eDown->Write("VV_CMS_scale_eDown" , TObject::kOverwrite);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
 
  // fix the TT template
  cout << "Now fixing the TT templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  fin.cd(channel+"Tau_SM2");
  TH1F* hTT                  = (TH1F*)gDirectory->Get("TT");
  TH1F* hTT_CMS_scale_tUp    = (TH1F*)gDirectory->Get("TT_CMS_scale_tUp");
  TH1F* hTT_CMS_scale_tDown  = (TH1F*)gDirectory->Get("TT_CMS_scale_tDown");
  TH1F* hTT_CMS_scale_jUp    = (TH1F*)gDirectory->Get("TT_CMS_scale_jUp");
  TH1F* hTT_CMS_scale_jDown  = (TH1F*)gDirectory->Get("TT_CMS_scale_jDown");
  TH1F* hTT_CMS_scale_eUp    = (TH1F*)gDirectory->Get("TT_CMS_scale_eUp");
  TH1F* hTT_CMS_scale_eDown  = (TH1F*)gDirectory->Get("TT_CMS_scale_eDown");

  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hTT_help                   = (TH1F*)gDirectory->Get("TT");
  TH1F* hTT_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("TT_CMS_scale_tUp");
  TH1F* hTT_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("TT_CMS_scale_tDown");
  float ratioUp   = (hTT_CMS_scale_tUp_help->Integral()   - hTT_help->Integral())/hTT_help->Integral();
  float ratioDown = (hTT_CMS_scale_tDown_help->Integral() - hTT_help->Integral())/hTT_help->Integral();
  cout << "scale_t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hTT_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hTT->Integral() / hTT_CMS_scale_tUp->Integral() );
  hTT_CMS_scale_tDown->Scale( ( 1+ratioDown )*hTT->Integral() / hTT_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hTT_help                   = (TH1F*)gDirectory->Get("TT");
  TH1F* hTT_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("TT_CMS_scale_jUp");
  TH1F* hTT_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("TT_CMS_scale_jDown");
  float ratioUp   = (hTT_CMS_scale_jUp_help->Integral()   - hTT_help->Integral())/hTT_help->Integral();
  float ratioDown = (hTT_CMS_scale_jDown_help->Integral() - hTT_help->Integral())/hTT_help->Integral();
  if(fixJetScaleForTT){
    ratioUp   =  jetScaleForTT;
    ratioDown = -jetScaleForTT;
  }
  cout << "scale_j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hTT_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hTT->Integral() / hTT_CMS_scale_jUp->Integral() );
  hTT_CMS_scale_jDown->Scale( ( 1+ratioDown )*hTT->Integral() / hTT_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hTT_help                   = (TH1F*)gDirectory->Get("TT");
  TH1F* hTT_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("TT_CMS_scale_eUp");
  TH1F* hTT_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("TT_CMS_scale_eDown");
  float ratioUp   = (hTT_CMS_scale_eUp_help->Integral()   - hTT_help->Integral())/hTT_help->Integral();
  float ratioDown = (hTT_CMS_scale_eDown_help->Integral() - hTT_help->Integral())/hTT_help->Integral();
  cout << "scale_e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hTT_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hTT->Integral() / hTT_CMS_scale_eUp->Integral() );
  hTT_CMS_scale_eDown->Scale( ( 1+ratioDown )*hTT->Integral() / hTT_CMS_scale_eDown->Integral() );

  fin.cd(channel+"Tau_SM2");
  hTT_CMS_scale_tUp->Write("TT_CMS_scale_tUp" ,     TObject::kOverwrite);
  hTT_CMS_scale_tDown->Write("TT_CMS_scale_tDown" , TObject::kOverwrite);
  hTT_CMS_scale_jUp->Write("TT_CMS_scale_jUp" ,     TObject::kOverwrite);
  hTT_CMS_scale_jDown->Write("TT_CMS_scale_jDown" , TObject::kOverwrite);
  hTT_CMS_scale_eUp->Write("TT_CMS_scale_eUp" ,     TObject::kOverwrite);
  hTT_CMS_scale_eDown->Write("TT_CMS_scale_eDown" , TObject::kOverwrite);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
 

  // fix the W template
  cout << "Now fixing the W templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  fin.cd(channel+"Tau_SM2");
  TH1F* hW                  = (TH1F*)gDirectory->Get("W");
  TH1F* hW_CMS_scale_tUp    = (TH1F*)gDirectory->Get("W_CMS_scale_tUp");
  TH1F* hW_CMS_scale_tDown  = (TH1F*)gDirectory->Get("W_CMS_scale_tDown");
  TH1F* hW_CMS_scale_jUp    = (TH1F*)gDirectory->Get("W_CMS_scale_jUp");
  TH1F* hW_CMS_scale_jDown  = (TH1F*)gDirectory->Get("W_CMS_scale_jDown");
  TH1F* hW_CMS_scale_eUp    = (TH1F*)gDirectory->Get("W_CMS_scale_eUp");
  TH1F* hW_CMS_scale_eDown  = (TH1F*)gDirectory->Get("W_CMS_scale_eDown");
  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hW_help                   = (TH1F*)gDirectory->Get("W");
  TH1F* hW_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("W_CMS_scale_tUp");
  TH1F* hW_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("W_CMS_scale_tDown");
  float ratioUp   = (hW_CMS_scale_tUp_help->Integral()   - hW_help->Integral())/hW_help->Integral();
  float ratioDown = (hW_CMS_scale_tDown_help->Integral() - hW_help->Integral())/hW_help->Integral();
  cout << "scale_t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hW_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hW->Integral() / hW_CMS_scale_tUp->Integral() );
  hW_CMS_scale_tDown->Scale( ( 1+ratioDown )*hW->Integral() / hW_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hW_help                   = (TH1F*)gDirectory->Get("W");
  TH1F* hW_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("W_CMS_scale_jUp");
  TH1F* hW_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("W_CMS_scale_jDown");
  float ratioUp   = (hW_CMS_scale_jUp_help->Integral()   - hW_help->Integral())/hW_help->Integral();
  float ratioDown = (hW_CMS_scale_jDown_help->Integral() - hW_help->Integral())/hW_help->Integral();
  cout << "scale_j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  if(fixJetScaleForW){
    ratioUp   =  jetScaleForW;
    ratioDown = -jetScaleForW;
  }
  hW_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hW->Integral() / hW_CMS_scale_jUp->Integral() );
  hW_CMS_scale_jDown->Scale( ( 1+ratioDown )*hW->Integral() / hW_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hW_help                   = (TH1F*)gDirectory->Get("W");
  TH1F* hW_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("W_CMS_scale_eUp");
  TH1F* hW_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("W_CMS_scale_eDown");
  float ratioUp   = (hW_CMS_scale_eUp_help->Integral()   - hW_help->Integral())/hW_help->Integral();
  float ratioDown = (hW_CMS_scale_eDown_help->Integral() - hW_help->Integral())/hW_help->Integral();
  cout << "scale_e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hW_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hW->Integral() / hW_CMS_scale_eUp->Integral() );
  hW_CMS_scale_eDown->Scale( ( 1+ratioDown )*hW->Integral() / hW_CMS_scale_eDown->Integral() );

  fin.cd(channel+"Tau_SM2");
  hW_CMS_scale_tUp->Write("W_CMS_scale_tUp" , TObject::kOverwrite);
  hW_CMS_scale_tDown->Write("W_CMS_scale_tDown" , TObject::kOverwrite);
  hW_CMS_scale_jUp->Write("W_CMS_scale_jUp" , TObject::kOverwrite);
  hW_CMS_scale_jDown->Write("W_CMS_scale_jDown" , TObject::kOverwrite);
  hW_CMS_scale_eUp->Write("W_CMS_scale_eUp" , TObject::kOverwrite);
  hW_CMS_scale_eDown->Write("W_CMS_scale_eDown" , TObject::kOverwrite);

  //fix the ZL 
  cout << "Now fixing the ZL templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  fin.cd(channel+"Tau_SM2");
  TH1F* hZL                  = (TH1F*)gDirectory->Get("ZL");
  TH1F* hZL_CMS_scale_tUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_tUp");
  TH1F* hZL_CMS_scale_tDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_tDown");
  TH1F* hZL_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
  TH1F* hZL_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");
  TH1F* hZL_CMS_scale_eUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_eUp");
  TH1F* hZL_CMS_scale_eDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_eDown");
  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZL_help                   = (TH1F*)gDirectory->Get("ZL");
  TH1F* hZL_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("ZL_CMS_scale_tUp");
  TH1F* hZL_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("ZL_CMS_scale_tDown");
  float ratioUp   = (hZL_CMS_scale_tUp_help->Integral()   - hZL_help->Integral())/hZL_help->Integral();
  float ratioDown = (hZL_CMS_scale_tDown_help->Integral() - hZL_help->Integral())/hZL_help->Integral();
  cout << "scale t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZL_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hZL->Integral() / hZL_CMS_scale_tUp->Integral() );
  hZL_CMS_scale_tDown->Scale( ( 1+ratioDown )*hZL->Integral() / hZL_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZL_help                   = (TH1F*)gDirectory->Get("ZL");
  TH1F* hZL_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
  TH1F* hZL_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");
  float ratioUp   = (hZL_CMS_scale_jUp_help->Integral()   - hZL_help->Integral())/hZL_help->Integral();
  float ratioDown = (hZL_CMS_scale_jDown_help->Integral() - hZL_help->Integral())/hZL_help->Integral();  
  if(fixJetScaleForZL){
    ratioUp   =  jetScaleForZL;
    ratioDown = -jetScaleForZL;
  }
  cout << "scale_j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZL_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hZL->Integral() / hZL_CMS_scale_jUp->Integral() );
  hZL_CMS_scale_jDown->Scale( ( 1+ratioDown )*hZL->Integral() / hZL_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZL_help                   = (TH1F*)gDirectory->Get("ZL");
  TH1F* hZL_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("ZL_CMS_scale_eUp");
  TH1F* hZL_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("ZL_CMS_scale_eDown");
  float ratioUp   = (hZL_CMS_scale_eUp_help->Integral()   - hZL_help->Integral())/hZL_help->Integral();
  float ratioDown = (hZL_CMS_scale_eDown_help->Integral() - hZL_help->Integral())/hZL_help->Integral();
  cout << "scale_e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZL_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hZL->Integral() / hZL_CMS_scale_eUp->Integral() );
  hZL_CMS_scale_eDown->Scale( ( 1+ratioDown )*hZL->Integral() / hZL_CMS_scale_eDown->Integral() );
  
  fin.cd(channel+"Tau_SM2");
  hZL_CMS_scale_tUp->Write("ZL_CMS_scale_tUp" , TObject::kOverwrite);
  hZL_CMS_scale_tDown->Write("ZL_CMS_scale_tDown" , TObject::kOverwrite);
  hZL_CMS_scale_jUp->Write("ZL_CMS_scale_jUp" , TObject::kOverwrite);
  hZL_CMS_scale_jDown->Write("ZL_CMS_scale_jDown" , TObject::kOverwrite);
  hZL_CMS_scale_eUp->Write("ZL_CMS_scale_eUp" , TObject::kOverwrite);
  hZL_CMS_scale_eDown->Write("ZL_CMS_scale_eDown" , TObject::kOverwrite);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////


  //fix the ZJ 
  cout << "Now fixing the ZJ templates..." << endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  fin.cd(channel+"Tau_SM2");
  TH1F* hZJ                  = (TH1F*)gDirectory->Get("ZJ");
  TH1F* hZJ_CMS_scale_tUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tUp");
  TH1F* hZJ_CMS_scale_tDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tDown");
  TH1F* hZJ_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
  TH1F* hZJ_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");
  TH1F* hZJ_CMS_scale_eUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_eUp");
  TH1F* hZJ_CMS_scale_eDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_eDown");
  // SCALE-T
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZJ_help                   = (TH1F*)gDirectory->Get("ZJ");
  TH1F* hZJ_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tUp");
  TH1F* hZJ_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tDown");
  float ratioUp   = (hZJ_CMS_scale_tUp_help->Integral()   - hZJ_help->Integral())/hZJ_help->Integral();
  float ratioDown = (hZJ_CMS_scale_tDown_help->Integral() - hZJ_help->Integral())/hZJ_help->Integral();  
  cout << "scale t ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZJ_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hZJ->Integral() / hZJ_CMS_scale_tUp->Integral() );
  hZJ_CMS_scale_tDown->Scale( ( 1+ratioDown )*hZJ->Integral() / hZJ_CMS_scale_tDown->Integral() );
  // SCALE-J
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZJ_help                   = (TH1F*)gDirectory->Get("ZJ");
  TH1F* hZJ_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
  TH1F* hZJ_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");
  float ratioUp   = (hZJ_CMS_scale_jUp_help->Integral()   - hZJ_help->Integral())/hZJ_help->Integral();
  float ratioDown = (hZJ_CMS_scale_jDown_help->Integral() - hZJ_help->Integral())/hZJ_help->Integral();
  if(fixJetScaleForZJ){
    ratioUp   =  jetScaleForZJ;
    ratioDown = -jetScaleForZJ;
  }
  cout << "scale j ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZJ_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hZJ->Integral() / hZJ_CMS_scale_jUp->Integral() );
  hZJ_CMS_scale_jDown->Scale( ( 1+ratioDown )*hZJ->Integral() / hZJ_CMS_scale_jDown->Integral() );
  // SCALE-E
  fin.cd(channel+"Tau_SMpre2");
  TH1F* hZJ_help                   = (TH1F*)gDirectory->Get("ZJ");
  TH1F* hZJ_CMS_scale_eUp_help     = (TH1F*)gDirectory->Get("ZJ_CMS_scale_eUp");
  TH1F* hZJ_CMS_scale_eDown_help   = (TH1F*)gDirectory->Get("ZJ_CMS_scale_eDown");
  float ratioUp   = (hZJ_CMS_scale_eUp_help->Integral()   - hZJ_help->Integral())/hZJ_help->Integral();
  float ratioDown = (hZJ_CMS_scale_eDown_help->Integral() - hZJ_help->Integral())/hZJ_help->Integral();
  cout << "scale e ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
  hZJ_CMS_scale_eUp->Scale(   ( 1+ratioUp   )*hZJ->Integral() / hZJ_CMS_scale_eUp->Integral() );
  hZJ_CMS_scale_eDown->Scale( ( 1+ratioDown )*hZJ->Integral() / hZJ_CMS_scale_eDown->Integral() );
  
  fin.cd(channel+"Tau_SM2");
  hZJ_CMS_scale_tUp->Write("ZJ_CMS_scale_tUp" , TObject::kOverwrite);
  hZJ_CMS_scale_tDown->Write("ZJ_CMS_scale_tDown" , TObject::kOverwrite);
  hZJ_CMS_scale_jUp->Write("ZJ_CMS_scale_jUp" , TObject::kOverwrite);
  hZJ_CMS_scale_jDown->Write("ZJ_CMS_scale_jDown" , TObject::kOverwrite);
  hZJ_CMS_scale_eUp->Write("ZJ_CMS_scale_eUp" , TObject::kOverwrite);
  hZJ_CMS_scale_eDown->Write("ZJ_CMS_scale_eDown" , TObject::kOverwrite);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // fix the SM templates
  vector<int> hMass;
  hMass.push_back(105);
  hMass.push_back(110);
  hMass.push_back(115);
  hMass.push_back(120);
  hMass.push_back(125);
  hMass.push_back(130);
  hMass.push_back(135);
  hMass.push_back(140);

  for(int k=0; k<hMass.size(); k++ ){

    cout << "Now fixing the SM" << hMass[k] <<  " templates..." << endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    fin.cd(channel+"Tau_SM2");
    TH1F* hSM                  = (TH1F*)gDirectory->Get(Form("SM%d",hMass[k]));
    TH1F* hSM_CMS_scale_tUp    = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_tUp"  ,hMass[k]));
    TH1F* hSM_CMS_scale_tDown  = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_tDown",hMass[k]));
    TH1F* hSM_CMS_scale_jUp    = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_jUp"  ,hMass[k]));
    TH1F* hSM_CMS_scale_jDown  = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_jDown",hMass[k]));
  
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hSM_help                   = (TH1F*)gDirectory->Get(Form("SM%d",hMass[k]));
    TH1F* hSM_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_tUp",hMass[k]));
    TH1F* hSM_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_tDown",hMass[k]));
    float ratioUp   = (hSM_CMS_scale_tUp_help->Integral()   - hSM_help->Integral())/hSM_help->Integral();
    float ratioDown = (hSM_CMS_scale_tDown_help->Integral() - hSM_help->Integral())/hSM_help->Integral();
    cout << "scale_t from SMpre2 ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
    hSM_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hSM->Integral() / hSM_CMS_scale_tUp->Integral() );
    hSM_CMS_scale_tDown->Scale( ( 1+ratioDown )*hSM->Integral() / hSM_CMS_scale_tDown->Integral() );
    
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hSM_help                   = (TH1F*)gDirectory->Get(Form("SM%d",hMass[k]));
    TH1F* hSM_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_jUp",hMass[k]));
    TH1F* hSM_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get(Form("SM%d_CMS_scale_jDown",hMass[k]));
    float ratioUp   = (hSM_CMS_scale_jUp_help->Integral()   - hSM_help->Integral())/hSM_help->Integral();
    float ratioDown = (hSM_CMS_scale_jDown_help->Integral() - hSM_help->Integral())/hSM_help->Integral();
    cout << "scale_j from SMpre2 ==> Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;
    if(fixJetScaleForSM){
      cout << "Up/Down from scale_j " << ratioUp << "/" << ratioDown ;
      ratioUp   =  jetScaleForSM;
      ratioDown = -jetScaleForSM;
      cout << " changed to " << ratioUp << "/" << ratioDown << endl;
    }
    hSM_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hSM->Integral() / hSM_CMS_scale_jUp->Integral() );
    hSM_CMS_scale_jDown->Scale( ( 1+ratioDown )*hSM->Integral() / hSM_CMS_scale_jDown->Integral() );
    
    // overwrite:
    fin.cd(channel+"Tau_SM2");
    hSM_CMS_scale_tUp->Write(Form("SM%d_CMS_scale_tUp",hMass[k]) ,     TObject::kOverwrite);
    hSM_CMS_scale_tDown->Write(Form("SM%d_CMS_scale_tDown",hMass[k]) , TObject::kOverwrite);
    hSM_CMS_scale_jUp->Write(Form("SM%d_CMS_scale_jUp",hMass[k]) ,     TObject::kOverwrite);
    hSM_CMS_scale_jDown->Write(Form("SM%d_CMS_scale_jDown",hMass[k]) , TObject::kOverwrite);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
  }


  if(onlyJetMorphingZ && (string(variable.Data())).find("NSVfit")!=string::npos){
    
    // if the jet-part of the efficiency is taken from data
    // then, the morphing should affect only the shape is taken from data
    
    cout << "Now making all DY templates with same normalization when CMS_scale_j is varied" << endl;

    fin.cd(channel+"Tau_SM2");
    TH1F* hZTT                  = (TH1F*)gDirectory->Get("ZTT");
    TH1F* hZTT_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jUp");
    TH1F* hZTT_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jDown");

    hZTT_CMS_scale_jUp->Scale(hZTT->Integral()/hZTT_CMS_scale_jUp->Integral());
    hZTT_CMS_scale_jDown->Scale(hZTT->Integral()/hZTT_CMS_scale_jDown->Integral());
    hZTT_CMS_scale_jUp->Write("ZTT_CMS_scale_jUp" ,     TObject::kOverwrite);
    hZTT_CMS_scale_jDown->Write("ZTT_CMS_scale_jDown" , TObject::kOverwrite);
    
    TH1F* hZL                  = (TH1F*)gDirectory->Get("ZL");
    TH1F* hZL_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
    TH1F* hZL_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");
    
    hZL_CMS_scale_jUp->Scale(hZL->Integral()/hZL_CMS_scale_jUp->Integral());
    hZL_CMS_scale_jDown->Scale(hZL->Integral()/hZL_CMS_scale_jDown->Integral());
    hZL_CMS_scale_jUp->Write("ZL_CMS_scale_jUp" ,     TObject::kOverwrite);
    hZL_CMS_scale_jDown->Write("ZL_CMS_scale_jDown" , TObject::kOverwrite);

    TH1F* hZJ                  = (TH1F*)gDirectory->Get("ZJ");
    TH1F* hZJ_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
    TH1F* hZJ_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");

    hZJ_CMS_scale_jUp->Scale(hZJ->Integral()/hZJ_CMS_scale_jUp->Integral());
    hZJ_CMS_scale_jDown->Scale(hZJ->Integral()/hZJ_CMS_scale_jDown->Integral());   
    hZJ_CMS_scale_jUp->Write("ZJ_CMS_scale_jUp" ,     TObject::kOverwrite);
    hZJ_CMS_scale_jDown->Write("ZJ_CMS_scale_jDown" , TObject::kOverwrite);
   

    fin.cd(channel+"Tau_SM0");
    TH1F* hZTT                  = (TH1F*)gDirectory->Get("ZTT");
    TH1F* hZTT_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jUp");
    TH1F* hZTT_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZTT_CMS_scale_jDown");

    hZTT_CMS_scale_jUp->Scale(hZTT->Integral()/hZTT_CMS_scale_jUp->Integral());
    hZTT_CMS_scale_jDown->Scale(hZTT->Integral()/hZTT_CMS_scale_jDown->Integral());

    TH1F* hZL                  = (TH1F*)gDirectory->Get("ZL");
    TH1F* hZL_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
    TH1F* hZL_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");

    hZL_CMS_scale_jUp->Scale(hZL->Integral()/hZL_CMS_scale_jUp->Integral());
    hZL_CMS_scale_jDown->Scale(hZL->Integral()/hZL_CMS_scale_jDown->Integral());

    TH1F* hZJ                  = (TH1F*)gDirectory->Get("ZJ");
    TH1F* hZJ_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
    TH1F* hZJ_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");

    hZJ_CMS_scale_jUp->Scale(hZJ->Integral()/hZJ_CMS_scale_jUp->Integral());
    hZJ_CMS_scale_jDown->Scale(hZJ->Integral()/hZJ_CMS_scale_jDown->Integral());

    hZL_CMS_scale_jUp->Write("ZL_CMS_scale_jUp" ,       TObject::kOverwrite);
    hZL_CMS_scale_jDown->Write("ZL_CMS_scale_jDown" ,   TObject::kOverwrite);
    hZJ_CMS_scale_jUp->Write("ZJ_CMS_scale_jUp" ,       TObject::kOverwrite);
    hZJ_CMS_scale_jDown->Write("ZJ_CMS_scale_jDown" ,   TObject::kOverwrite);
    hZTT_CMS_scale_jUp->Write("ZTT_CMS_scale_jUp" ,     TObject::kOverwrite);
    hZTT_CMS_scale_jDown->Write("ZTT_CMS_scale_jDown" , TObject::kOverwrite);
    
  }




  fin.Close();

}
