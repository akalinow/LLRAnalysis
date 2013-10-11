#include "TFile.h"
#include "TTree.h"
#include "TString.h"


void drawHistos(TCanvas * C, TString filename, TString category, TTree* Tmine, TTree* Tother,TString var, int nbins, float xmin, float xmax, TString selection, TString myGroup, TString myRootFile, TString group, TString groupRootFile,TString mySel="1",TString groupSel="1"){
  TH1F* Hmine = new TH1F(TString("Hmine")+var,"",nbins,xmin,xmax); 
  Hmine->GetYaxis()->SetTitle(category);
  Hmine->GetXaxis()->SetTitle(var);
  Hmine->SetLineColor(1);
  Hmine->SetStats(0);
  TH1F* Hother = new TH1F(TString("Hother")+var,"",nbins,xmin,xmax); 
  Hother->GetYaxis()->SetTitle(category);
  Hother->GetXaxis()->SetTitle(var);
  Hother->SetLineColor(2);
  Hother->SetStats(0);

  TText TXmine;
  TXmine.SetTextColor(1);
  TXmine.SetTextSize(.04);
  TText TXother;
  TXother.SetTextColor(2);
  TXother.SetTextSize(.04);

  Tmine->Draw(var+">>"+Hmine->GetName(),selection+"*("+mySel+")");
  Tother->Draw(var+">>"+Hother->GetName(),selection+"*("+groupSel+")");

  ////Draw one histogram on top of the other
  C->Clear();
  //Hmine->Scale(1./Hmine->Integral());
  //Hother->Scale(1./Hother->Integral()); 
  //Hother->Scale(968134./688134.); //GGH e-tau
  if(Hmine->GetMaximum()>Hother->GetMaximum())
    Hmine->GetYaxis()->SetRangeUser(0,Hmine->GetMaximum()*1.1);
  else Hmine->GetYaxis()->SetRangeUser(0,Hother->GetMaximum()*1.1);
  Hmine->Draw("hist");
  Hother->Draw("histsame");

//   ///Draw the difference of the historgrams
//   TH1F*HDiff=(TH1F*)Hmine->Clone("HDiff");
//   HDiff->Add(Hother,-1);
//   int max= abs(HDiff->GetMaximum())>abs( HDiff->GetMinimum()) ?   abs(HDiff->GetMaximum()): abs( HDiff->GetMinimum());
//   HDiff->GetYaxis()->SetRangeUser(-2*(max>0?max:1),2*(max>0?max:1));
//   HDiff->Draw("hist");
//   TLine line;
//   line.DrawLine(HDiff->GetXaxis()->GetXmin(),0,HDiff->GetXaxis()->GetXmax(),0);

  //Print the integrals of the histograms a the top
  //TXmine.DrawTextNDC(.2,.965,myGroup+"_"+myRootFile+": "+(long)(Hmine->Integral(0,Hmine->GetNbinsX()+1)));
  //TXother.DrawTextNDC(.2,.93,group+"_"+groupRootFile+": "+(long)(Hother->Integral(0,Hother->GetNbinsX()+1)));
  TXmine.DrawTextNDC(.2,.965,myGroup+" : "+(long)(Hmine->Integral(0,Hmine->GetNbinsX()+1)));
  TXother.DrawTextNDC(.2,.93,group+": "+(long)(Hother->Integral(0,Hother->GetNbinsX()+1)));
  C->Print(filename);

  delete Hmine;
  delete Hother;
}

void compareSync(TString myGroup,TString myPath,TString myRootFile, TString myTree, TString group, TString groupPath, TString groupRootFile, TString groupTree,TString mySel="1",TString groupSel="1"){

//   TFile Fmine("/afs/cern.ch/user/b/benitezj/public/HTTSync/July28/eTau2012_VBFSync.root");
//   TTree*Tmine=(TTree*)Fmine.Get("TauCheck");
//   TFile Fother("/afs/cern.ch/user/s/swanson/public/Sync52XEleTau/eleTauPreselection.root");
//   TTree*Tother=(TTree*)Fother.Get("eventTree");

  TFile Fmine(myPath+"/"+myRootFile+".root");
  TTree*Tmine=(TTree*)Fmine.Get(myTree.Data());
  TFile Fother(groupPath+"/"+groupRootFile+".root");
  TTree*Tother=(TTree*)Fother.Get(groupTree.Data());

  
  ////////////////

  cout<<"Mine: "<<Fmine.GetName()<<endl;
  cout<<"Other: "<<Fother.GetName()<<endl;
  TCanvas C;

  TString filename=myGroup+"_"+myRootFile+"_"+group+"_"+groupRootFile+"_diff.pdf";
  C.Print(filename+"[");  


  //inclusive
  TString selection="1";
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"run",140,190000,204000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"npu",50,0,50,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"npv",50,0,50,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"pt_1",100,0,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"eta_1",60,-3,3,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"iso_1",60,-.02,0.12,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"pt_2",100,0,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"eta_2",60,-3,3,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,"inclusive",Tmine,Tother,"met",20,0,200,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,"inclusive",Tmine,Tother,"metphi",30,-3.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvamet",50,0,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvametphi",35,-3.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov00",50,0,2500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov01",50,0,2500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov10",50,0,2500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mvacov11",50,0,2500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"uPerp",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"uParl",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metParl",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metPerp",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metSigmaParl",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metSigmaPerp",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metPullParl",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"metPullPerp",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"m_sv",60,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"mt_1",200,-100,100,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  drawHistos(&C,filename,"inclusive",Tmine,Tother,"puweight",25,-.1,0.9,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"effweight",80,.7,1.3,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,"inclusive",Tmine,Tother,"eventweight",50,0,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  ///Jets
  selection="(njets>=1)";
  drawHistos(&C,filename,selection,Tmine,Tother,"jpt_1",100,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"jeta_1",100,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //
  selection="(njets>=2)";
  drawHistos(&C,filename,selection,Tmine,Tother,"jpt_2",100,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"jeta_2",100,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"mjj",100,0,3000,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"jdeta",100,0,10,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"visjeteta",100,0,10,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"ptvis",100,0,500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"njetingap",4,-0.5,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,selection,Tmine,Tother,"jdphi",100,0,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,selection,Tmine,Tother,"dijetpt",100,0,500,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,selection,Tmine,Tother,"hdijetphi",100,0,3.5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  //drawHistos(&C,filename,selection,Tmine,Tother,"mva",40,-1,1.001,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  /////b-jets
  selection="(nbtag>=1)";
  drawHistos(&C,filename,selection,Tmine,Tother,"bpt",100,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  drawHistos(&C,filename,selection,Tmine,Tother,"beta",100,-5,5,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  ///////////////////categories
  TString vbfcut="(njets>=2&&njetingap==0&&mjj>500&&abs(jdeta)>3.5)";
  TString notvbfcut=TString("(!")+vbfcut+")";
  TString boostcut="(njets>=1&&nbtag==0)";
  TString notboostcut=TString("(!")+boostcut+")";
  TString bjetcut="(njets<2&&nbtag>=1)";
  TString notbjetcut=TString("(!")+bjetcut+")";
  TString SMcut[7];
  SMcut[0]="(njets==0&&nbtag==0)";
  SMcut[2]=notvbfcut+"*"+boostcut;
  SMcut[4]=vbfcut;
  SMcut[5]=notvbfcut+"*"+notboostcut+"*"+bjetcut;
  
  selection=SMcut[0];
  drawHistos(&C,filename,TString("0-jet : ")+selection,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[2];
  drawHistos(&C,filename,"Boosted",Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[4];
  drawHistos(&C,filename,TString("VBF : ")+selection,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[5];
  drawHistos(&C,filename,"B-jet",Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  //with puweight
  TString weight="(puweight)";
  selection=SMcut[0]+"*"+weight;
  drawHistos(&C,filename,TString("0-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[2]+"*"+weight;
  drawHistos(&C,filename,TString("Boosted ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[4]+"*"+weight;
  drawHistos(&C,filename,TString("VBF ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[5]+"*"+weight;
  drawHistos(&C,filename,TString("B-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  //with trigger weight
  TString weight="(puweight*effweight)";
  selection=SMcut[0]+"*"+weight;
  drawHistos(&C,filename,TString("0-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[2]+"*"+weight;
  drawHistos(&C,filename,TString("Boosted ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[4]+"*"+weight;
  drawHistos(&C,filename,TString("VBF ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[5]+"*"+weight;
  drawHistos(&C,filename,TString("B-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);

  //with trigger weight and mT cut
  TString weight="(puweight*effweight)*(mt_1<20.)";
  selection=SMcut[0]+"*"+weight;
  drawHistos(&C,filename,TString("0-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[2]+"*"+weight;
  drawHistos(&C,filename,TString("Boosted ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[4]+"*"+weight;
  drawHistos(&C,filename,TString("VBF ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);
  selection=SMcut[5]+"*"+weight;
  drawHistos(&C,filename,TString("B-jet ")+weight,Tmine,Tother,"m_sv",30,0,300,selection,myGroup,myRootFile,group,groupRootFile,mySel,groupSel);


  C.Print(filename+"]");
  gROOT->ProcessLine(".q");
}
