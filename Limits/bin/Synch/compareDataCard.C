//#include <iosstream>
//#include <iomanip>

void drawHisto(TCanvas* C,TString Group1,TFile* F1,TString Group2,TFile* F2,TString channel,TString category,TString sample,bool PrintDiff){
  C->Clear();
  
  TH1F* H1 = (TH1F*) F1->Get(channel+"_"+category+"/"+sample);
  TH1F* H2 = (TH1F*) F2->Get(channel+"_"+category+"/"+sample);
  
  if(!H1||!H2||H1->Integral()!=H1->Integral()||H2->Integral()!=H2->Integral()){

    if(H1&&H1->Integral()==H1->Integral())H1->Draw("histpe");
    if(H2&&H2->Integral()==H2->Integral())H2->Draw("histpe");

    TText cat;
    cat.DrawTextNDC(.2,.5,channel+"_"+category+"/"+sample);
    if(!H1||H1->Integral()!=H1->Integral()) cat.DrawTextNDC(.2,.4,Group1+" is missing or 0");
    if(!H2||H2->Integral()!=H2->Integral()) cat.DrawTextNDC(.2,.4,Group2+" is missing or 0");

    if(PrintDiff)printf("| %.1f ",100);
    C->Print(TString(C->GetName())+".pdf");
    return;
  }
//   cout<<channel<<" "<<category<<" "<<sample<<endl;
//   cout<<H1->Integral()<<" "<<H2->Integral()<<endl;
//   TText cat;
//   cat.DrawTextNDC(.2,.5,channel+"_"+category+"/"+sample);
//   C->Print(TString(C->GetName())+".pdf");
//   return;


 
  TPad pad1("pad1","",0,0.2,1,1);
  TPad pad2("pad2","",0,0,1,0.2);
    
  ////////////////////////////////////////////
  pad1.cd();


  //H1->SetTitle("");
  H1->SetTitle(channel+"  "+category+"  "+sample);
  H1->GetXaxis()->SetTitle("mass");
  H1->GetYaxis()->SetTitle("");
  H1->GetYaxis()->SetRangeUser(0,(H1->GetMaximum()>H2->GetMaximum() ? H1->GetMaximum() : H2->GetMaximum())*1.2);
  H1->SetLineWidth(3);
  H1->SetLineColor(1);
  H1->SetFillColor(0);
  H1->Draw("histpe");
  H2->SetMarkerSize(0.01);
  H2->SetLineWidth(3);
  H2->SetLineColor(2);
  H2->SetFillColor(0);
  H2->Draw("histesame");
  
  TText TXmine;
  TXmine.SetTextColor(1);
  TXmine.SetTextSize(.04);
  TText TXother;
  TXother.SetTextColor(2);
  TXother.SetTextSize(.04);
  TText TXdiff;
  TXdiff.SetTextColor(4);
  TXdiff.SetTextSize(.03);
  char yield1[100];
  sprintf(yield1,"%.2f",H1->Integral(1,H1->GetNbinsX()));
  char yield2[100];
  sprintf(yield2,"%.2f",H2->Integral(1,H2->GetNbinsX()));
  TXmine.DrawTextNDC(.55,.845,Group1+" : "+yield1);
  TXother.DrawTextNDC(.55,.81,Group2+" : "+yield2);
  char txt[100];
  float diff=100*2*fabs(H1->Integral(1,H1->GetNbinsX())-H2->Integral(1,H2->GetNbinsX()))/(H1->Integral(1,H1->GetNbinsX())+H2->Integral(1,H2->GetNbinsX()));
  sprintf(txt,"Difference = %.1f",diff);
  TXdiff.DrawTextNDC(.25,.85,TString(txt)+"%");

  ////////////////////////////////////////////
  pad2.cd();
  ///Draw the ratio of the historgrams
  TH1F*HDiff=(TH1F*)H2->Clone("HDiff");
  HDiff->Divide(H1);
  HDiff->GetYaxis()->SetRangeUser(0.8,1.2);
  HDiff->GetYaxis()->SetNdivisions(3);
  HDiff->GetYaxis()->SetLabelSize(0.1);
  HDiff->GetYaxis()->SetTitleSize(0.1);
  HDiff->GetYaxis()->SetTitleOffset(0.5);
  //HDiff->GetYaxis()->SetTitle(myGroup + " / " + group);
  HDiff->GetYaxis()->SetTitle("Ratio");
  HDiff->GetXaxis()->SetNdivisions(-1);
  HDiff->GetXaxis()->SetTitle("");
  HDiff->GetXaxis()->SetLabelSize(0.0001);
  HDiff->SetMarkerSize(0.1);
  HDiff->Draw("histpe");
  TLine line;
  line.DrawLine(HDiff->GetXaxis()->GetXmin(),1,HDiff->GetXaxis()->GetXmax(),1);




  C->Clear();
  pad1.Draw();
  pad2.Draw();



  //cout<<setiosflags(ios::fixed)<<precision(2);
  //cout<<"| "<<diff<<" "<<endl;
  //if(PrintDiff)printf("| %.1f ",diff);
  C->Print(TString(C->GetName())+".pdf");

  //delete H1;
  //delete H2;
  delete HDiff;
}



void drawSignalDiff(TCanvas* C,TString Group1,TFile* F1,TString Group2,TFile* F2,TString channel,TString category,TString signal){
  //     drawSignalDiff(         C,        Group1,       F1,         Group2      ,F2,        channel,       "boost_high",     "ggH");
  C->Clear();
  
  TGraph G1;
  TGraph G2;
  float max=0.;
  for(Int_t i=0;i<8;i++){
    long mass=110+i*5;
    TH1F* H1 = (TH1F*) F1->Get(channel+"_"+category+"/"+signal+mass);
    TH1F* H2 = (TH1F*) F2->Get(channel+"_"+category+"/"+signal+mass);
    G1.SetPoint(i,mass,H1->Integral());
    G2.SetPoint(i,mass,H2->Integral());
    if(H1->Integral()>max)max=H1->Integral();
  }
  
  G1.GetYaxis()->SetRangeUser(0.,max*1.1);
  G1.SetTitle(category+"  "+signal);
  G1.GetYaxis()->SetTitle("signal yield");
  G1.GetXaxis()->SetTitle("m_{H}");
  G1.Draw("ap");
  G2.SetLineColor(2);
  G2.Draw("lsame");


  TText TXmine;
  TXmine.SetTextColor(1);
  TXmine.SetTextSize(.04);
  TXmine.DrawTextNDC(.25,.845,Group1); 

  TText TXother;
  TXother.SetTextColor(2);
  TXother.SetTextSize(.04);
  TXother.DrawTextNDC(.25,.81,Group2);


  C->Print(TString(C->GetName())+".pdf");
}


void drawCategory(TCanvas* C,TString Group1, TFile* F1,TString Group2, TFile* F2,TString channel, TString category,bool PrintDiff){
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"data_obs",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"ZTT",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"W",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"QCD",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"TT",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"ZL",0&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"ZJ",0&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"ZLL",0&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"VV",0&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"ggH125",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"qqH125",1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,category,"VH125",0&&PrintDiff);

}


void drawSample(TCanvas* C,TString Group1, TFile* F1,TString Group2, TFile* F2,TString channel, TString sample,bool PrintDiff){
  drawHisto(C,Group1,F1,Group2,F2,channel,"0jet_low",sample,1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,"0jet_high",sample,1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,"boost_low",sample,1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,"boost_high",sample,1&&PrintDiff);
  drawHisto(C,Group1,F1,Group2,F2,channel,"vbf",sample,1&&PrintDiff);
}


void compareDataCard(
		     TString channel="muTau",
		     TString year="2011",
		     TString Group1="CERN",
		     TString Path1="/afs/cern.ch/user/b/benitezj/public/datacards/2011/Aug4",
		     TString File1="muTauSM_svfitmass",
		     TString Group2="Saclay",
		     TString Path2="/afs/cern.ch/user/b/bianchi/public/Roger/datacards2011v3",
		     TString File2="muTauSM"
		     ){

  TFile F1(Path1+"/"+File1+".root","read");
  TFile F2(Path2+"/"+File2+".root","read");
  
  F1.ls();
  F2.ls();

  TString fname="Diff_"+channel+"_"+year+"_"+Group1+"_"+Group2;
  TCanvas C(fname);
  
  C.Print(TString(C.GetName())+".pdf[");
  
  //cout<<" | 0jet_low Data | 0jet_low ZTT | 0jet_low W | 0jet_low QCD | 0jet_low TT | 0jet_low  ggH | 0jet_low  qqH | Boosted_high Data | Boosted_high ZTT | Boosted_high W | Boosted_high QCD | Boosted_high TT | Boosted_high ggH | Boosted_high qqH | VBF Data| VBF ZTT | VBF W | VBF QCD | VBF TT | VBF qqH | VBF ggH |"<<endl;


//   drawCategory(&C,Group1,&F1,Group2,&F2,channel,"0jet_low",1);
//   drawCategory(&C,Group1,&F1,Group2,&F2,channel,"0jet_high",0);
//   drawCategory(&C,Group1,&F1,Group2,&F2,channel,"boost_low",0);
//   drawCategory(&C,Group1,&F1,Group2,&F2,channel,"boost_high",1);
//   drawCategory(&C,Group1,&F1,Group2,&F2,channel,"vbf",1);



  drawSample(&C,Group1,&F1,Group2,&F2,channel,"data_obs",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZTT",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"W",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"QCD",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"TT",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZL",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZJ",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZLL",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"VV",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"ggH125",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"qqH125",0);
  drawSample(&C,Group1,&F1,Group2,&F2,channel,"VH125",0);


//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"boost_high","ggH");
//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"boost_high","qqH");
//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"boost_high","VH");

//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"vbf","ggH");
//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"vbf","qqH");
//   drawSignalDiff(&C,Group1,&F1,Group2,&F2,channel,"vbf","VH");

  //drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZTT_CMS_scale_t_mutau_8TeVUp",0);
  //drawSample(&C,Group1,&F1,Group2,&F2,channel,"ZTT_CMS_scale_t_mutau_8TeVDown",0);


  C.Print(TString(C.GetName())+".pdf]");
  
  //cout<<"|"<<endl;

  gROOT->ProcessLine(".q");
}

