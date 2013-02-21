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
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "Oct2011"
	     ){


  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ << " analysis " << analysis_ << " and bin " << bin_ << endl;
  TFile* fin = new TFile(Form("histograms/%s/muTau_mH%d_%s_%s_%s.root", outputDir.Data(), mH_, bin_.c_str() , analysis_.c_str(), variable_.c_str()), "READ");

  TFile* fin_jUp   = new TFile(Form("histograms/%s/muTau_mH%d_%s_JetUp_%s.root",  outputDir.Data(), mH_, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_jDown = new TFile(Form("histograms/%s/muTau_mH%d_%s_JetDown_%s.root", outputDir.Data(), mH_, bin_.c_str() , variable_.c_str()), "READ");

  string binNameSpace = "";
  if(bin_.find("inclusive")!=string::npos)      
    binNameSpace =  "inclusive";
  else if(bin_.find("novbf")!=string::npos) 
    binNameSpace =  "SM0";
  else if(bin_.find("boost")!=string::npos) 
    binNameSpace =  "SM1";
  else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) 
    binNameSpace =  "SM2";
  else if(bin_.find("twoJets")!=string::npos) 
    binNameSpace =  "SMpre2";
  else if(bin_.find("oneJet")!=string::npos) 
    binNameSpace =  "SMpre2a";

  TFile* fTemplOut = new TFile(Form("datacards/%s/muTauSM_%s.root",outputDir.Data(), variable_.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "_CMS_scale_tUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "_CMS_scale_tDown";
  else if(analysis_.find("MuUp")!=string::npos)
    suffix = "_CMS_scale_mUp";
  else if(analysis_.find("MuDown")!=string::npos)
    suffix = "_CMS_scale_mDown";
  else if(analysis_.find("JetUp")!=string::npos)
    suffix = "_CMS_scale_jUp";
  else if(analysis_.find("JetDown")!=string::npos)
    suffix = "_CMS_scale_jDown";
  
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

    if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hZtt"))->Write(Form("ZTT%s"       ,suffix.c_str()));
    if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hQCD"))->Write(Form("QCD%s"       ,suffix.c_str()));
    if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
    if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
      ((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
    if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
    if(dir->FindObjectAny(Form("VBF%d%s"         ,mH_,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hSgn1"))->Write(Form("VBF%d%s",mH_  ,suffix.c_str()));
    if(dir->FindObjectAny(Form("SM%d%s"          , mH_,suffix.c_str()))==0 )
      ((TH1F*)fin->Get("hSgn2"))->Write(Form("SM%d%s" ,mH_,suffix.c_str()));
  }

  fTemplOut->Close();
  
  // edit the datacards only for the nominal analysis
  if(analysis_!="") return;


  ifstream in;

  char* c = new char[1000];
  in.open(Form("templates/muTau_%s_template.txt",     binNameSpace.c_str()));
  ofstream out(Form("datacards/%s/muTau_%s_mH%d_%s.txt", outputDir.Data(), binNameSpace.c_str(), mH_, variable_.c_str()));
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
	  string rate = "rate                                           ";
	  string space = "              ";
	  out << rate ;
	  out <<          ((TH1F*)fin->Get("hSgn2"))->Integral() 
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
	else if(line.find("CMS_scale_j")!=string::npos && variable_.find("NSVfit")==string::npos && (binNameSpace.find("SM2")!=string::npos || binNameSpace.find("SM1")!=string::npos)){
	  float VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hSgn1"))->Integral()/((TH1F*)fin->Get("hSgn1"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hSgn1"))->Integral()/((TH1F*)fin->Get("hSgn1"))->Integral()))
				    );
	  float SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hSgn2"))->Integral()/((TH1F*)fin->Get("hSgn2"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hSgn2"))->Integral()/((TH1F*)fin->Get("hSgn2"))->Integral()))
				    );
	  float ZTTrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral()))
				    );
	  float TTrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral()))
				    );
	  float VVrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral()))
				    );
	  string space      = "                   ";
	  string longspace  = "                                ";
	  string shortspace = "     ";
	  out << "CMS_scale_j";
	  out << longspace << "lnN" << shortspace;
	  //out <<          string(Form("%.2f",SMrel));
	  out          <<  "1.20";
	  out << space << string(Form("%.2f",VBFrel));
	  out << space << "-";
	  out << space << "-";
	  out << space << string(Form("%.2f",ZTTrel));
	  out << space << "-";
	  out << space << "-";
	  out << space << string(Form("%.2f",TTrel));
	  out << space << string(Form("%.2f",VVrel));
	  out << space << "JEC uncertainty";
	  out << endl;
	}
	else if(line.find("CMS_scale_j")!=string::npos && variable_.find("NSVfit")==string::npos && binNameSpace.find("SM0")!=string::npos){
	  float VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hSgn1"))->Integral()/((TH1F*)fin->Get("hSgn1"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hSgn1"))->Integral()/((TH1F*)fin->Get("hSgn1"))->Integral())));
	  string space      = "                      ";
	  string longspace  = "                      ";
	  string shortspace = "     ";
	  out << "CMS_scale_j";
	  out << longspace << "lnN" << shortspace;
	  out <<          "-" ;
	  out << space << string(Form("%.2f",2-VBFrel));
	  out << space << "-";
	  out << space << "-";
	  out << space << "-";
	  out << space << "-";
	  out << space << "-";
	  out << space << "-";
	  out << space << "-";
	  out << space << "JEC uncertainty";
	  out << endl;
	}
	else if(line.find("CMS_scale_j")!=string::npos && variable_.find("NSVfit")!=string::npos){
          line.replace( line.find("CMS_scale_j") , 12  , "CMS_scale_j "  );
	  out << line << endl;
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
	else if(line.find( "CMS_htt_muTau_SM2_QCDSyst")!=string::npos || line.find( "CMS_htt_muTau_SM1_QCDSyst")!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.2f",1.0+((TH1F*)fin->Get("hParameters"))->GetBinContent(7)))  );
	  out << line << endl;
	}
	else if(line.find( "CMS_htt_muTau_SM2_WNorm")!=string::npos || line.find( "CMS_htt_muTau_SM1_WNorm")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hW"))->GetEntries()))  );
          line.replace( line.find("YYY") , 3 , string(Form("%.3f",((TH1F*)fin->Get("hW"))->Integral()/((TH1F*)fin->Get("hW"))->GetEntries() ))  );
          out << line << endl;
        }
	else if(line.find( "CMS_htt_muTau_SM0_WNorm")!=string::npos){
          line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(2)))  );
          line.replace( line.find("YYY") , 3 , string(Form("%.3f",((TH1F*)fin->Get("hParameters"))->GetBinContent(1)))  );
          out << line << endl;
        }
	else if(line.find( "CMS_htt_muTau_SM2_ZJetFakeTauNorm" )!=string::npos || line.find( "CMS_htt_muTau_SM1_ZJetFakeTauNorm" )!=string::npos){
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
	else if(line.find( "CMS_htt_muTau_SM2_ZLeptonFakeTauNorm" )!=string::npos || line.find( "CMS_htt_muTau_SM1_ZLeptonFakeTauNorm" )!=string::npos){
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



void produceAll(  TString outputDir = "Oct2011" ){

  vector<string> variables;
  vector<int> mH;

  //variables.push_back("diTauVisMass");
  variables.push_back("diTauNSVfitMass");

  //mH.push_back(105);
  //mH.push_back(110);
  //mH.push_back(115);
  mH.push_back(120);
  //mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);
  //mH.push_back(140);

  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){

      produce(mH[j],variables[i], ""        , "vbf", outputDir);
      produce(mH[j],variables[i], "TauUp"   , "vbf", outputDir);
      produce(mH[j],variables[i], "TauDown" , "vbf", outputDir);
      produce(mH[j],variables[i], "JetUp"   , "vbf", outputDir);
      produce(mH[j],variables[i], "JetDown" , "vbf", outputDir);

      produce(mH[j],variables[i], ""        , "boost", outputDir);
      produce(mH[j],variables[i], "TauUp"   , "boost", outputDir);
      produce(mH[j],variables[i], "TauDown" , "boost", outputDir);
      produce(mH[j],variables[i], "JetUp"   , "boost", outputDir);
      produce(mH[j],variables[i], "JetDown" , "boost", outputDir);
      
      produce(mH[j],variables[i], ""        , "novbf", outputDir);
      produce(mH[j],variables[i], "TauUp"   , "novbf", outputDir);
      produce(mH[j],variables[i], "TauDown" , "novbf", outputDir);
      produce(mH[j],variables[i], "JetUp"   , "novbf", outputDir);
      produce(mH[j],variables[i], "JetDown" , "novbf", outputDir);

      produce(mH[j],variables[i], ""        , "twoJets", outputDir);
      produce(mH[j],variables[i], "TauUp"   , "twoJets", outputDir);
      produce(mH[j],variables[i], "TauDown" , "twoJets", outputDir);
      produce(mH[j],variables[i], "JetUp"   , "twoJets", outputDir);
      produce(mH[j],variables[i], "JetDown" , "twoJets", outputDir);
      
      produce(mH[j],variables[i], ""        , "oneJet", outputDir);
      produce(mH[j],variables[i], "TauUp"   , "oneJet", outputDir);
      produce(mH[j],variables[i], "TauDown" , "oneJet", outputDir);
      produce(mH[j],variables[i], "JetUp"   , "oneJet", outputDir);
      produce(mH[j],variables[i], "JetDown" , "oneJet", outputDir);
    }

  }

}
