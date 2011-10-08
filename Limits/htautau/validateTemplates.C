

{

  // TString inputDir = "datacards";
  TString inputDir = ".";
  TString bin = "eleTau_SM2";

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->Divide(6,4);

  TFile f(inputDir+"/eleTauSM_diTauNSVfitMass.root","READ");

  f.cd(bin);

  TString analysis = "";

  TH1F* hQCD = (TH1F*)gDirectory->Get("QCD"+analysis);
  TH1F* hW   = (TH1F*)gDirectory->Get("W"+analysis);
  TH1F* hZTT = (TH1F*)gDirectory->Get("ZTT"+analysis);
  TH1F* hTT  = (TH1F*)gDirectory->Get("TT"+analysis);
  TH1F* hVV  = (TH1F*)gDirectory->Get("VV"+analysis);
  TH1F* hZL  = (TH1F*)gDirectory->Get("ZL"+analysis);
  TH1F* hZJ  = (TH1F*)gDirectory->Get("ZJ"+analysis);
  TH1F* hSM105    = (TH1F*)gDirectory->Get("SM105"+analysis);
  TH1F* hSM110    = (TH1F*)gDirectory->Get("SM110"+analysis);
  TH1F* hSM115    = (TH1F*)gDirectory->Get("SM115"+analysis);
  TH1F* hSM120    = (TH1F*)gDirectory->Get("SM120"+analysis);
  TH1F* hSM125    = (TH1F*)gDirectory->Get("SM125"+analysis);
  TH1F* hSM130    = (TH1F*)gDirectory->Get("SM130"+analysis);
  TH1F* hSM135    = (TH1F*)gDirectory->Get("SM135"+analysis);
  TH1F* hSM140    = (TH1F*)gDirectory->Get("SM140"+analysis);
  TH1F* hVBF105    = (TH1F*)gDirectory->Get("VBF105"+analysis);
  TH1F* hVBF110    = (TH1F*)gDirectory->Get("VBF110"+analysis);
  TH1F* hVBF115    = (TH1F*)gDirectory->Get("VBF115"+analysis);
  TH1F* hVBF120    = (TH1F*)gDirectory->Get("VBF120"+analysis);
  TH1F* hVBF125    = (TH1F*)gDirectory->Get("VBF125"+analysis);
  TH1F* hVBF130    = (TH1F*)gDirectory->Get("VBF130"+analysis);
  TH1F* hVBF135    = (TH1F*)gDirectory->Get("VBF135"+analysis);
  TH1F* hVBF140    = (TH1F*)gDirectory->Get("VBF140"+analysis);

  analysis = "_CMS_scale_jUp";
 
  TH1F* hQCD_CMS_scale_jUp = (TH1F*)gDirectory->Get("QCD"+analysis);
  TH1F* hW_CMS_scale_jUp   = (TH1F*)gDirectory->Get("W"+analysis);
  TH1F* hZTT_CMS_scale_jUp = (TH1F*)gDirectory->Get("ZTT"+analysis);
  TH1F* hTT_CMS_scale_jUp  = (TH1F*)gDirectory->Get("TT"+analysis);
  TH1F* hVV_CMS_scale_jUp  = (TH1F*)gDirectory->Get("VV"+analysis);
  TH1F* hZL_CMS_scale_jUp  = (TH1F*)gDirectory->Get("ZL"+analysis);
  TH1F* hZJ_CMS_scale_jUp  = (TH1F*)gDirectory->Get("ZJ"+analysis);
  TH1F* hSM105_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM105"+analysis);
  TH1F* hSM110_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM110"+analysis);
  TH1F* hSM115_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM115"+analysis);
  TH1F* hSM120_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM120"+analysis);
  TH1F* hSM125_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM125"+analysis);
  TH1F* hSM130_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM130"+analysis);
  TH1F* hSM135_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM135"+analysis);
  TH1F* hSM140_CMS_scale_jUp    = (TH1F*)gDirectory->Get("SM140"+analysis);
  TH1F* hVBF105_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF105"+analysis);
  TH1F* hVBF110_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF110"+analysis);
  TH1F* hVBF115_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF115"+analysis);
  TH1F* hVBF120_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF120"+analysis);
  TH1F* hVBF125_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF125"+analysis);
  TH1F* hVBF130_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF130"+analysis);
  TH1F* hVBF135_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF135"+analysis);
  TH1F* hVBF140_CMS_scale_jUp    = (TH1F*)gDirectory->Get("VBF140"+analysis);

  analysis = "_CMS_scale_jDown";
 
  TH1F* hQCD_CMS_scale_jDown = (TH1F*)gDirectory->Get("QCD"+analysis);
  TH1F* hW_CMS_scale_jDown   = (TH1F*)gDirectory->Get("W"+analysis);
  TH1F* hZTT_CMS_scale_jDown = (TH1F*)gDirectory->Get("ZTT"+analysis);
  TH1F* hTT_CMS_scale_jDown  = (TH1F*)gDirectory->Get("TT"+analysis);
  TH1F* hVV_CMS_scale_jDown  = (TH1F*)gDirectory->Get("VV"+analysis);
  TH1F* hZL_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZL"+analysis);
  TH1F* hZJ_CMS_scale_jDown  = (TH1F*)gDirectory->Get("ZJ"+analysis);
  TH1F* hSM105_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM105"+analysis);
  TH1F* hSM110_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM110"+analysis);
  TH1F* hSM115_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM115"+analysis);
  TH1F* hSM120_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM120"+analysis);
  TH1F* hSM125_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM125"+analysis);
  TH1F* hSM130_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM130"+analysis);
  TH1F* hSM135_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM135"+analysis);
  TH1F* hSM140_CMS_scale_jDown    = (TH1F*)gDirectory->Get("SM140"+analysis);
  TH1F* hVBF105_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF105"+analysis);
  TH1F* hVBF110_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF110"+analysis);
  TH1F* hVBF115_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF115"+analysis);
  TH1F* hVBF120_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF120"+analysis);
  TH1F* hVBF125_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF125"+analysis);
  TH1F* hVBF130_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF130"+analysis);
  TH1F* hVBF135_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF135"+analysis);
  TH1F* hVBF140_CMS_scale_jDown    = (TH1F*)gDirectory->Get("VBF140"+analysis);


  c0->cd(1);
  hQCD_CMS_scale_jUp->SetLineColor(kRed);
  hQCD_CMS_scale_jDown->SetLineColor(kBlue);
  hQCD_CMS_scale_jUp->Draw();
  hQCD->Draw("SAME");
  hQCD_CMS_scale_jDown->Draw("SAME");

  unsigned int it = 1;
  c0->cd(it+1);
  hZTT_CMS_scale_jUp->SetLineColor(kRed);
  hZTT_CMS_scale_jDown->SetLineColor(kBlue);
  hZTT_CMS_scale_jUp->Draw();
  hZTT->Draw("SAME");
  hZTT_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+2);
  hZJ_CMS_scale_jUp->SetLineColor(kRed);
  hZJ_CMS_scale_jDown->SetLineColor(kBlue);
  hZJ_CMS_scale_jUp->Draw();
  hZJ->Draw("SAME");
  hZJ_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+3);
  hZL_CMS_scale_jUp->SetLineColor(kRed);
  hZL_CMS_scale_jDown->SetLineColor(kBlue);
  hZL_CMS_scale_jUp->Draw();
  hZL->Draw("SAME");
  hZL_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+4);
  hTT_CMS_scale_jUp->SetLineColor(kRed);
  hTT_CMS_scale_jDown->SetLineColor(kBlue);
  hTT_CMS_scale_jUp->Draw();
  hTT->Draw("SAME");
  hTT_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+5);
  hVV_CMS_scale_jUp->SetLineColor(kRed);
  hVV_CMS_scale_jDown->SetLineColor(kBlue);
  hVV_CMS_scale_jUp->Draw();
  hVV->Draw("SAME");
  hVV_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+6);
  hSM105_CMS_scale_jUp->SetLineColor(kRed);
  hSM105_CMS_scale_jDown->SetLineColor(kBlue);
  hSM105_CMS_scale_jUp->Draw();
  hSM105->Draw("SAME");
  hSM105_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+7);
  hVBF105_CMS_scale_jUp->SetLineColor(kRed);
  hVBF105_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF105_CMS_scale_jUp->Draw();
  hVBF105->Draw("SAME");
  hVBF105_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+8);
  hSM110_CMS_scale_jUp->SetLineColor(kRed);
  hSM110_CMS_scale_jDown->SetLineColor(kBlue);
  hSM110_CMS_scale_jUp->Draw();
  hSM110->Draw("SAME");
  hSM110_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+9);
  hVBF110_CMS_scale_jUp->SetLineColor(kRed);
  hVBF110_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF110_CMS_scale_jUp->Draw();
  hVBF110->Draw("SAME");
  hVBF110_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+10);
  hSM115_CMS_scale_jUp->SetLineColor(kRed);
  hSM115_CMS_scale_jDown->SetLineColor(kBlue);
  hSM115_CMS_scale_jUp->Draw();
  hSM115->Draw("SAME");
  hSM115_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+11);
  hVBF115_CMS_scale_jUp->SetLineColor(kRed);
  hVBF115_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF115_CMS_scale_jUp->Draw();
  hVBF115->Draw("SAME");
  hVBF115_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+12);
  hSM120_CMS_scale_jUp->SetLineColor(kRed);
  hSM120_CMS_scale_jDown->SetLineColor(kBlue);
  hSM120_CMS_scale_jUp->Draw();
  hSM120->Draw("SAME");
  hSM120_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+13);
  hVBF120_CMS_scale_jUp->SetLineColor(kRed);
  hVBF120_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF120_CMS_scale_jUp->Draw();
  hVBF120->Draw("SAME");
  hVBF120_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+14);
  hSM125_CMS_scale_jUp->SetLineColor(kRed);
  hSM125_CMS_scale_jDown->SetLineColor(kBlue);
  hSM125_CMS_scale_jUp->Draw();
  hSM125->Draw("SAME");
  hSM125_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+15);
  hVBF125_CMS_scale_jUp->SetLineColor(kRed);
  hVBF125_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF125_CMS_scale_jUp->Draw();
  hVBF125->Draw("SAME");
  hVBF125_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+16);
  hSM130_CMS_scale_jUp->SetLineColor(kRed);
  hSM130_CMS_scale_jDown->SetLineColor(kBlue);
  hSM130_CMS_scale_jUp->Draw();
  hSM130->Draw("SAME");
  hSM130_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+17);
  hVBF130_CMS_scale_jUp->SetLineColor(kRed);
  hVBF130_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF130_CMS_scale_jUp->Draw();
  hVBF130->Draw("SAME");
  hVBF130_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+18);
  hSM135_CMS_scale_jUp->SetLineColor(kRed);
  hSM135_CMS_scale_jDown->SetLineColor(kBlue);
  hSM135_CMS_scale_jUp->Draw();
  hSM135->Draw("SAME");
  hSM135_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+19);
  hVBF135_CMS_scale_jUp->SetLineColor(kRed);
  hVBF135_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF135_CMS_scale_jUp->Draw();
  hVBF135->Draw("SAME");
  hVBF135_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+20);
  hSM140_CMS_scale_jUp->SetLineColor(kRed);
  hSM140_CMS_scale_jDown->SetLineColor(kBlue);
  hSM140_CMS_scale_jUp->Draw();
  hSM140->Draw("SAME");
  hSM140_CMS_scale_jDown->Draw("SAME");

  c0->cd(it+21);
  hVBF140_CMS_scale_jUp->SetLineColor(kRed);
  hVBF140_CMS_scale_jDown->SetLineColor(kBlue);
  hVBF140_CMS_scale_jUp->Draw();
  hVBF140->Draw("SAME");
  hVBF140_CMS_scale_jDown->Draw("SAME");

  c0->cd();

  c0->Draw();

}
