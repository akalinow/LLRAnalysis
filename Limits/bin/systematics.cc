#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TSystem.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstring>

using namespace std;

void produce(   
	     int mH_=120,
	     string variable_  = "diTauSVFitMass",
	     string bin_       = "novbf",
	     string systematic_= "CMS_eff_m",
	     float shift_      = 2,
	     string type_      = "lnN"
	     ){

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

  ifstream in;

  char* c = new char[1000];
  in.open(Form("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/Limits/htautau/results/MuTau/muTau_%s_mH%d_%s.txt", 
	       binNameSpace.c_str(), mH_, variable_.c_str()));
  ofstream out(Form("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/Limits/htautau/results/MuTau/systematics/muTau_%s_mH%d_%s_%sX%.2f.txt", 
		    binNameSpace.c_str(), mH_, variable_.c_str(), systematic_.c_str(),shift_));
  out.precision(8);

  if(in.good()) cout << "File correctly open" << endl;

  while (in.good())
    {
      in.getline(c,1000,'\n');

      if (in.good()){

	string line(c);
	string newLine = line;
	if(line.find( systematic_ )!=string::npos){
	  size_t a = 0;
	  size_t b = 0;
	  while(a!=string::npos){
	    //a = line.find_first_of("1.",b);
	    a = line.find_first_of(".",b);
	    if(a==string::npos) continue;
	    //b = line.find_first_of(" " ,a);
	    b = line.find_first_of(" " ,a-1);
	    //string oldS = line.substr(a,b-a);
	    string oldS = line.substr(a-1,b-a+1);
	    string newS = oldS;
	    if(type_.find("lnN")!=string::npos)
	      newS = shift_>1e-06 ? string(Form("%.5f", 1+(atof(oldS.c_str())-1)*shift_   )) : 
		string(Form("%.5f", float(1.0)   )) ;
	    if(type_.find("shape")!=string::npos && type_.find("shapes")==string::npos)
	      newS = shift_>1e-06 ? string(Form("%.5f", (atof(oldS.c_str()))*shift_   )) :
		string(Form("%.5f", float(0.0)   )) ;
	    cout << systematic_ << ": " << atof(oldS.c_str()) << "==>" << 1+(atof(oldS.c_str())-1)*shift_ << endl;
	    //line.replace( a , 4 , newS  );
	    newLine.replace( a-1 , 6 , newS  );
	  }
	  cout << "Old line: \n" << line << endl;
	  cout << "New line: \n" << newLine << endl;
	}
	out << newLine << endl;
      }
    }


}


int main(int argc, const char* argv[])
{

  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  vector<int> mH;
  //mH.push_back(105);
  mH.push_back(110);
  //mH.push_back(115);
  mH.push_back(120);
  //mH.push_back(125);
  mH.push_back(130);
  //mH.push_back(135);
  mH.push_back(140);
  //mH.push_back(145);

  //produce();
  //return 0;

  for(unsigned int i = 0 ; i < mH.size() ; i++){

    // CMS_eff_t
    produce(mH[i],"diTauSVFitMass","novbf","CMS_eff_t",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_eff_t",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_eff_t",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_eff_t",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_eff_t",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_eff_t",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_eff_t",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_eff_t",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_eff_t",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_eff_t",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_eff_t",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_eff_t",1,"lnN");

    // lumi
    produce(mH[i],"diTauSVFitMass","novbf","lumi",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","lumi",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","lumi",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","lumi",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","lumi",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","lumi",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","lumi",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","lumi",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","lumi",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","lumi",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","lumi",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","lumi",1,"lnN");

    // CMS_htt_ttbarNorm
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ttbarNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ttbarNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ttbarNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ttbarNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ttbarNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ttbarNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ttbarNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ttbarNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ttbarNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ttbarNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ttbarNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ttbarNorm",1,"lnN");

    // CMS_htt_ztt_extrap
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ztt_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ztt_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ztt_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_ztt_extrap",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ztt_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ztt_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ztt_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_ztt_extrap",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ztt_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ztt_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ztt_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_ztt_extrap",1,"lnN");

    // CMS_htt_W_extrap
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_W_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_W_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_W_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_W_extrap",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_W_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_W_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_W_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_W_extrap",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_W_extrap",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_W_extrap",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_W_extrap",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_W_extrap",1,"lnN");

    // CMS_scale_met
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_met",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_met",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_met",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_met",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_met",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_met",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_met",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_met",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_met",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_met",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_met",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_met",1,"lnN");

    // CMS_scale_t
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_t",2,"shape");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_t",0.5,"shape");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_t",0.0,"shape");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_t",1,"shape");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_t",2,"shape");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_t",0.5,"shape");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_t",0.0,"shape");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_t",1,"shape");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_t",2,"shape");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_t",0.5,"shape");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_t",0.0,"shape");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_t",1,"shape");

    // CMS_scale_jet
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_j",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_j",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_j",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_scale_j",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_j",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_j",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_j",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_scale_j",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_j",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_j",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_j",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_scale_j",1,"lnN");

    // QCDSyst
    produce(mH[i],"diTauSVFitMass","novbf","QCDSyst",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDSyst",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDSyst",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDSyst",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDSyst",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDSyst",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDSyst",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDSyst",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDSyst",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDSyst",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDSyst",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDSyst",1,"lnN");

    // CMS_htt_WNorm
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_WNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_WNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_WNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_WNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_WNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_WNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_WNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_WNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_WNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_WNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_WNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_WNorm",1,"lnN");

    // CMS_htt_zttNorm
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_zttNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_zttNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_zttNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","CMS_htt_zttNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_zttNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_zttNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_zttNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","CMS_htt_zttNorm",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_zttNorm",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_zttNorm",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_zttNorm",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","CMS_htt_zttNorm",1,"lnN");

    // QCDscale_ggHlin
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_ggHlin",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_ggHlin",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_ggHlin",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_ggHlin",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_ggHlin",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_ggHlin",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_ggHlin",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_ggHlin",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_ggHlin",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_ggHlin",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_ggHlin",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_ggHlin",1,"lnN");

    // QCDscale_qqH
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_qqH",2,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_qqH",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_qqH",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","novbf","QCDscale_qqH",1,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_qqH",2,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_qqH",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_qqH",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","boost","QCDscale_qqH",1,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_qqH",2,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_qqH",0.5,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_qqH",0.0,"lnN");
    produce(mH[i],"diTauSVFitMass","vbf","QCDscale_qqH",1,"lnN");


  }

}
