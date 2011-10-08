



{

  TString channel = "mu";
  TString variable = "diTauVisMass";

  std::vector<TString> analysis;
  analysis.push_back(TString(""));
  analysis.push_back(TString("_CMS_scale_tUp"));
  analysis.push_back(TString("_CMS_scale_tDown"));
  analysis.push_back(TString("_CMS_scale_jUp"));
  analysis.push_back(TString("_CMS_scale_jDown"));
  //analysis.push_back(TString("_CMS_scale_eUp"));
  //analysis.push_back(TString("_CMS_scale_eDown"));

  TFile fin("datacards/"+channel+"TauSM_"+variable+".root","UPDATE");

  for(int k = 0 ; k < analysis.size() ; k++){

    fin.cd(channel+"Tau_SM0");
    TH1F* hQCD_new = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hW_new   = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hVV_new  = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    TH1F* hZL_new  = (TH1F*)gDirectory->Get("ZL"+analysis[k]);
    TH1F* hZJ_new  = (TH1F*)gDirectory->Get("ZJ"+analysis[k]);
    
    fin.cd(channel+"Tau_SM2");
    TH1F* hQCD_old = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hW_old   = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hVV_old  = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    TH1F* hZL_old  = (TH1F*)gDirectory->Get("ZL"+analysis[k]);
    TH1F* hZJ_old  = (TH1F*)gDirectory->Get("ZJ"+analysis[k]);

    fin.cd(channel+"Tau_SMpre2");
    TH1F* hQCD_help = (TH1F*)gDirectory->Get("QCD"+analysis[k]);
    TH1F* hW_help   = (TH1F*)gDirectory->Get("W"+analysis[k]);
    TH1F* hVV_help  = (TH1F*)gDirectory->Get("VV"+analysis[k]);
    //TH1F* hZL_help  = (TH1F*)gDirectory->Get("ZL"+analysis[k]);
    //TH1F* hZJ_help  = (TH1F*)gDirectory->Get("ZJ"+analysis[k]);

    // directory to overwrite:
    fin.cd(channel+"Tau_SM2");

    ////////////////////////////////////////////////////////
    float QCD_old = hQCD_old->Integral();
    hQCD_old->Reset();
    hQCD_old->Add(hQCD_help, QCD_old/hQCD_help->Integral());
    hQCD_old->Write("QCD"+analysis[k] , TObject::kOverwrite);
    
    float W_old = hW_old->Integral();
    hW_old->Reset();
    hW_old->Add(hW_help, W_old/hW_help->Integral());
    hW_old->Write("W"+analysis[k] , TObject::kOverwrite);
    
    float VV_old = hVV_old->Integral();
    hVV_old->Reset();
    hVV_old->Add(hVV_help, VV_old/hVV_help->Integral());
    hVV_old->Write("VV"+analysis[k] , TObject::kOverwrite);
    
    float ZL_old = hZL_old->Integral();
    hZL_old->Reset();
    hZL_old->Add(hZL_new, ZL_old/hZL_new->Integral());
    hZL_old->Write("ZL"+analysis[k] , TObject::kOverwrite);
    
    float ZJ_old = hZJ_old->Integral();
    hZJ_old->Reset();
    hZJ_old->Add(hZJ_new, ZJ_old/hZJ_new->Integral());
    hZJ_old->Write("ZJ"+analysis[k] , TObject::kOverwrite);
    ///////////////////////////////////////////////////////

  }


  //fix the ZL and ZJ templates

  /////////////////////////////////////////////////////////
  fin.cd(channel+"Tau_SM2");
  TH1F* hZL                  = (TH1F*)gDirectory->Get("ZL");
  TH1F* hZL_CMS_scale_tUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_tUp");
  TH1F* hZL_CMS_scale_tDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_tDown");
  TH1F* hZL_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
  TH1F* hZL_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");
  if( (hZL_CMS_scale_tUp->Integral()-hZL->Integral())*(hZL_CMS_scale_tDown->Integral()-hZL->Integral()) >0 ){
    cout << "Templates for ZL do not cover the systematics (probably due to poor statistics) ==> using the templates from SMpre2" << endl;
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hZL_help                   = (TH1F*)gDirectory->Get("ZL");
    TH1F* hZL_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("ZL_CMS_scale_tUp");
    TH1F* hZL_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("ZL_CMS_scale_tDown");
    float ratioUp   = (hZL_CMS_scale_tUp_help->Integral()   - hZL_help->Integral())/hZL_help->Integral();
    float ratioDown = (hZL_CMS_scale_tDown_help->Integral() - hZL_help->Integral())/hZL_help->Integral();

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    if(ratioUp*ratioDown>0){
      cout << "Still undefined... taking the max among the two" << endl;
      float max = TMath::Max( TMath::Abs(ratioUp), TMath::Abs(ratioDown) );
      ratioUp   = max*TMath::Abs(ratioUp)/ratioUp;
      ratioDown = max*TMath::Abs(ratioDown)/ratioDown;
    }

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    hZL_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hZL->Integral() / hZL_CMS_scale_tUp->Integral() );
    hZL_CMS_scale_tDown->Scale( ( 1+ratioDown )*hZL->Integral() / hZL_CMS_scale_tDown->Integral() );
  } 
  if( (hZL_CMS_scale_jUp->Integral()-hZL->Integral())*(hZL_CMS_scale_jDown->Integral()-hZL->Integral()) >0 ){
    cout << "Templates for ZL do not cover the systematics (probably due to poor statistics) ==> using the templates from SMpre2" << endl;
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hZL_help                   = (TH1F*)gDirectory->Get("ZL");
    TH1F* hZL_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("ZL_CMS_scale_jUp");
    TH1F* hZL_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("ZL_CMS_scale_jDown");
    float ratioUp   = (hZL_CMS_scale_jUp_help->Integral()   - hZL_help->Integral())/hZL_help->Integral();
    float ratioDown = (hZL_CMS_scale_jDown_help->Integral() - hZL_help->Integral())/hZL_help->Integral();

    if(ratioUp*ratioDown>0){
      cout << "Still undefined... taking the max among the two" << endl;
      float max = TMath::Max( TMath::Abs(ratioUp), TMath::Abs(ratioDown) );
      ratioUp   = max*TMath::Abs(ratioUp)/ratioUp;
      ratioDown = max*TMath::Abs(ratioDown)/ratioDown;
    }

    hZL_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hZL->Integral() / hZL_CMS_scale_jUp->Integral() );
    hZL_CMS_scale_jDown->Scale( ( 1+ratioDown )*hZL->Integral() / hZL_CMS_scale_jDown->Integral() );
  }
  fin.cd(channel+"Tau_SM2");
  hZL_CMS_scale_tUp->Write("ZL_CMS_scale_tUp" , TObject::kOverwrite);
  hZL_CMS_scale_tDown->Write("ZL_CMS_scale_tDown" , TObject::kOverwrite);
  hZL_CMS_scale_jUp->Write("ZL_CMS_scale_jUp" , TObject::kOverwrite);
  hZL_CMS_scale_jDown->Write("ZL_CMS_scale_jDown" , TObject::kOverwrite);
  /////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////
  fin.cd(channel+"Tau_SM2");
  TH1F* hZJ                  = (TH1F*)gDirectory->Get("ZJ");
  TH1F* hZJ_CMS_scale_tUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tUp");
  TH1F* hZJ_CMS_scale_tDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tDown");
  TH1F* hZJ_CMS_scale_jUp    = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
  TH1F* hZJ_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");
  if( (hZJ_CMS_scale_tUp->Integral()-hZJ->Integral())*(hZJ_CMS_scale_tDown->Integral()-hZJ->Integral()) >0 ){
    cout << "Templates for ZJ do not cover the systematics (probably due to poor statistics) ==> using the templates from SMpre2" << endl;
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hZJ_help                   = (TH1F*)gDirectory->Get("ZJ");
    TH1F* hZJ_CMS_scale_tUp_help     = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tUp");
    TH1F* hZJ_CMS_scale_tDown_help   = (TH1F*)gDirectory->Get("ZJ_CMS_scale_tDown");
    float ratioUp   = (hZJ_CMS_scale_tUp_help->Integral()   - hZJ_help->Integral())/hZJ_help->Integral();
    float ratioDown = (hZJ_CMS_scale_tDown_help->Integral() - hZJ_help->Integral())/hZJ_help->Integral();

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    if(ratioUp*ratioDown>0){
      cout << "Still undefined... taking the max among the two" << endl;
      float max = TMath::Max( TMath::Abs(ratioUp), TMath::Abs(ratioDown) );
      ratioUp   = max*TMath::Abs(ratioUp)/ratioUp;
      ratioDown = max*TMath::Abs(ratioDown)/ratioDown;
    }

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    hZJ_CMS_scale_tUp->Scale(   ( 1+ratioUp   )*hZJ->Integral() / hZJ_CMS_scale_tUp->Integral() );
    hZJ_CMS_scale_tDown->Scale( ( 1+ratioDown )*hZJ->Integral() / hZJ_CMS_scale_tDown->Integral() );
  } 
  if( (hZJ_CMS_scale_jUp->Integral()-hZJ->Integral())*(hZJ_CMS_scale_jDown->Integral()-hZJ->Integral()) >0 ){
    cout << "Templates for ZJ do not cover the systematics (probably due to poor statistics) ==> using the templates from SMpre2" << endl;
    fin.cd(channel+"Tau_SMpre2");
    TH1F* hZJ_help                   = (TH1F*)gDirectory->Get("ZJ");
    TH1F* hZJ_CMS_scale_jUp_help     = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jUp");
    TH1F* hZJ_CMS_scale_jDown_help   = (TH1F*)gDirectory->Get("ZJ_CMS_scale_jDown");
    float ratioUp   = (hZJ_CMS_scale_jUp_help->Integral()   - hZJ_help->Integral())/hZJ_help->Integral();
    float ratioDown = (hZJ_CMS_scale_jDown_help->Integral() - hZJ_help->Integral())/hZJ_help->Integral();

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    if(ratioUp*ratioDown>0){
      cout << "Still undefined... taking the max among the two" << endl;
      float max = TMath::Max( TMath::Abs(ratioUp), TMath::Abs(ratioDown) );
      ratioUp   = max*TMath::Abs(ratioUp)/ratioUp;
      ratioDown = max*TMath::Abs(ratioDown)/ratioDown;
    }

    cout << "Ratio up " <<  ratioUp << " -- Ratio down " << ratioDown << endl;

    hZJ_CMS_scale_jUp->Scale(   ( 1+ratioUp   )*hZJ->Integral() / hZJ_CMS_scale_jUp->Integral() );
    hZJ_CMS_scale_jDown->Scale( ( 1+ratioDown )*hZJ->Integral() / hZJ_CMS_scale_jDown->Integral() );
  }
  fin.cd(channel+"Tau_SM2");
  hZJ_CMS_scale_tUp->Write("ZJ_CMS_scale_tUp" , TObject::kOverwrite);
  hZJ_CMS_scale_tDown->Write("ZJ_CMS_scale_tDown" , TObject::kOverwrite);
  hZJ_CMS_scale_jUp->Write("ZJ_CMS_scale_jUp" , TObject::kOverwrite);
  hZJ_CMS_scale_jDown->Write("ZJ_CMS_scale_jDown" , TObject::kOverwrite);
  /////////////////////////////////////////////////////////


  fin.Close();

}
