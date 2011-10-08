
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>


void produce(   
	     int mH_=120,
	     string variable_ = "diTauVisMass",
	     string analysis_ = "",
	     string bin_= "inclusive"
	     ){

  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ << " analysis " << analysis_ << " and bin " << bin_ << endl;
  TFile* fin = new TFile(Form("histograms/muTau_mH%d_%s_%s_%s.root", mH_, bin_.c_str() , analysis_.c_str(), variable_.c_str()), "READ");


  string binNameSpace = "";
  if(bin_.find("inclusive")!=string::npos)      
    binNameSpace =  "inclusive";
  else if(bin_.find("novbf")!=string::npos) 
    binNameSpace =  "SM0";
  else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) 
    binNameSpace =  "SM2";
  else if(bin_.find("twoJets")!=string::npos) 
    binNameSpace =  "SMpre2";
  else if(bin_.find("oneJet")!=string::npos) 
    binNameSpace =  "SMpre2a";

  TFile* fTemplOut = new TFile(Form("datacards/muTauSM_%s.root",variable_.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "CMS_scale_tUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "CMS_scale_tDown";
  else if(analysis_.find("MuUp")!=string::npos)
    suffix = "CMS_scale_mUp";
  else if(analysis_.find("MuDown")!=string::npos)
    suffix = "CMS_scale_mDown";
  else if(analysis_.find("JetUp")!=string::npos)
    suffix = "CMS_scale_jUp";
  else if(analysis_.find("JetDown")!=string::npos)
    suffix = "CMS_scale_jDown";
  
  cout << "Adding histos with suffix " << suffix << endl;
  TString dirName( Form("muTau_%s",binNameSpace.c_str()) );
  
  if(! (fTemplOut->cd( dirName.Data()  ) ) ){
    
    cout << "Editing the directory for bin and variable " << binNameSpace << ", " << variable_ << endl;

    TDirectory* dir = fTemplOut->mkdir( dirName.Data() );
    dir->cd();

    ((TH1F*)fin->Get("hData"))->Write("data_obs");
    ((TH1F*)fin->Get("hSgn1"))->Write(Form("VBF%d%s",mH_,suffix.c_str()));
    ((TH1F*)fin->Get("hSgn2"))->Write(Form("SM%d%s" ,mH_,suffix.c_str()));
    ((TH1F*)fin->Get("hZtt"))->Write(Form("ZTT%s"       ,suffix.c_str()));
    ((TH1F*)fin->Get("hQCD"))->Write(Form("QCD%s"       ,suffix.c_str()));
    ((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
    ((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
    ((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
    ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
    ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
           
  }else{
    cout << "Directory is there, filling only higgs histos..." << endl;
    //write the higgs only
    TDirectory* dir = (TDirectory*)fTemplOut->Get( dirName.Data() );

    fTemplOut->cd( dirName.Data() );

    if(dir->FindObjectAny(Form("ZTT_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hZtt"))->Write(Form("ZTT_%s"       ,suffix.c_str()));
    if(dir->FindObjectAny(Form("QCD_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hQCD"))->Write(Form("QCD_%s"       ,suffix.c_str()));
    if(dir->FindObjectAny(Form("W_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hW"))->Write(Form("W_%s"           ,suffix.c_str()));
    if(dir->FindObjectAny(Form("ZJ_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ_%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("ZL_%s"       ,suffix.c_str()))==0 ) 
      ((TH1F*)fin->Get("hZmm"))->Write(Form("ZL_%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("TT_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT_%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("VV_%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV_%s"         ,suffix.c_str()));
    if(dir->FindObjectAny(Form("VBF%d_%s"         ,mH_,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hSgn1"))->Write(Form("VBF%d_%s",mH_  ,suffix.c_str()));
    if(dir->FindObjectAny(Form("SM%d_%s"          , mH_,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hSgn2"))->Write(Form("SM%d_%s" ,mH_,suffix.c_str()));
  }

  fTemplOut->Close();
  
  // edit the datacards only for the nominal analysis
  if(analysis_!="") return;


  ifstream in;

  char* c = new char[1000];
  in.open(Form("templates/muTau_%s_template.txt",  binNameSpace.c_str()));
  ofstream out(Form("datacards/muTau_%s_mH%d.txt", binNameSpace.c_str(), mH_));
  out.precision(8);

  while (in.good())
    {
      in.getline(c,1000,'\n');
      if (in.good()){

	string line(c);
	if(line.find("observation")!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f", ((TH1F*)fin->Get("hData"))->Integral()) )  );
	  out << line << endl;
	}
	else if(line.find("shapes")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("muTauSM_%s",variable_.c_str()))  );
	  out << line << endl;
	}
	else if(line.find("process")!=string::npos && line.find("VBF")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("%d",mH_))  );
          line.replace( line.find("YYY") , 3 , string(Form("%d",mH_))  );
          out << line << endl;
        }
	else if(line.find("rate")!=string::npos){
	  string rate = "rate                      ";
	  string space = "                ";
	  out << rate ;
	  out << space << ((TH1F*)fin->Get("hSgn2"))->Integral() 
	      << space << ((TH1F*)fin->Get("hSgn1"))->Integral()
	      << space << ((TH1F*)fin->Get("hZtt"))->Integral()
	      << space << ((TH1F*)fin->Get("hQCD"))->Integral()
	      << space << ((TH1F*)fin->Get("hW"))->Integral()
	      << space << ((TH1F*)fin->Get("hZmj"))->Integral()
	      << space << ((TH1F*)fin->Get("hZmm"))->Integral()
	      << space << ((TH1F*)fin->Get("hTTb"))->Integral()
	      << space << ((TH1F*)fin->Get("hVV"))->Integral()
	      << endl;
	}
	else if(line.find( "CMS_htt_muTau_SM2_ttbarNorm" )!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hTTb"))->GetEntries()))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.5f",((TH1F*)fin->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->GetEntries() ))  );
	  out << line << endl;
	}
	else if(line.find( "CMS_htt_muTau_SM2_DiBosonNorm" )!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hVV"))->GetEntries()))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.5f",((TH1F*)fin->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->GetEntries() ))  );
	  out << line << endl;
	}
	else if(line.find( Form("CMS_htt_muTau_%s_QCDNorm",binNameSpace.c_str()) )!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(5)))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(6)))  );
	  out << line << endl;
	}
	else if(line.find( "CMS_htt_muTau_SM2_QCDSyst")!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.2f",1.0+((TH1F*)fin->Get("hParameters"))->GetBinContent(7)))  );
	  out << line << endl;
	}
	else if(line.find( "CMS_htt_muTau_SM2_WNorm")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hW"))->GetEntries()))  );
          line.replace( line.find("YYY") , 3 , string(Form("%.3f",((TH1F*)fin->Get("hW"))->Integral()/((TH1F*)fin->Get("hW"))->GetEntries() ))  );
          out << line << endl;
        }
	else if(line.find( "CMS_htt_muTau_SM0_WNorm")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(2)))  );
          line.replace( line.find("YYY") , 3 , string(Form("%.3f",((TH1F*)fin->Get("hParameters"))->GetBinContent(1)))  );
          out << line << endl;
        }
	else if(line.find( "CMS_htt_muTau_SM2_ZJetFakeTauNorm" )!=string::npos){
	  if(((TH1F*)fin->Get("hZmj"))->GetEntries()>0){
	    line.replace( line.find("KKK") , 3 , "gmN"  );
	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hZmj"))->GetEntries()))  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hZmj"))->Integral()/((TH1F*)fin->Get("hZmj"))->GetEntries() ))  );
	  }
	  else{
	    line.replace( line.find("KKK") , 3 , "   "  );
	    line.replace( line.find("XXX") , 3 , "lnN"  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.2f", 2.00 ))  );
	  }
          out << line << endl;
        }
	else if(line.find( "CMS_htt_muTau_SM2_ZLeptonFakeTauNorm" )!=string::npos){
	  if(((TH1F*)fin->Get("hZmm"))->GetEntries()>0){
	    line.replace( line.find("KKK") , 3 , "gmN"  );
	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hZmm"))->GetEntries()))  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hZmm"))->Integral()/((TH1F*)fin->Get("hZmm"))->GetEntries() ))  );
	  }
	  else{
	    line.replace( line.find("KKK") , 3 , "   "  );
	    line.replace( line.find("XXX") , 3 , "lnN"  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.2f", 2.00 ))  );
	  }
          out << line << endl;
        }
	else{
	  out << c << endl;
	}
      
      }
 
   }


  return;

}



void produceAll(){

  produce(120,"diTauVisMass", ""        , "vbf");
  produce(120,"diTauVisMass", "TauUp"   , "vbf");
  produce(120,"diTauVisMass", "TauDown" , "vbf");
  produce(120,"diTauVisMass", "JetUp"   , "vbf");
  produce(120,"diTauVisMass", "JetDown" , "vbf");

  produce(120,"diTauVisMass", ""        , "novbf");
  produce(120,"diTauVisMass", "TauUp"   , "novbf");
  produce(120,"diTauVisMass", "TauDown" , "novbf");
  produce(120,"diTauVisMass", "JetUp"   , "novbf");
  produce(120,"diTauVisMass", "JetDown" , "novbf");

  produce(120,"diTauVisMass", ""        , "twoJets");
  produce(120,"diTauVisMass", "TauUp"   , "twoJets");
  produce(120,"diTauVisMass", "TauDown" , "twoJets");
  produce(120,"diTauVisMass", "JetUp"   , "twoJets");
  produce(120,"diTauVisMass", "JetDown" , "twoJets");

  produce(120,"diTauVisMass", ""        , "oneJet");
  produce(120,"diTauVisMass", "TauUp"   , "oneJet");
  produce(120,"diTauVisMass", "TauDown" , "oneJet");
  produce(120,"diTauVisMass", "JetUp"   , "oneJet");
  produce(120,"diTauVisMass", "JetDown" , "oneJet");
}
