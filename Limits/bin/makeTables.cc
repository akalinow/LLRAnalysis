#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>

#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"

using namespace std;

static std::map<string,string> MAP = std::map<string,string>();

void queryText( TString stream_ = "MuTau", TString outputDir_   = "Dec2011/iter3", string selection_ = "inclusive",
	       std::map<string,string>& lMapInclusive_ = MAP){

  float yield = 0;
  float error = 0;
  float signal= 0;
  float signalError = 0;

  ifstream is;
  char* c  = new char[50];

  is.open(Form("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/Limits/htautau/histograms/%s/yields%s_mH120_%s_.txt",outputDir_.Data(),stream_.Data(),selection_.c_str()));
  if(is.is_open()) 
    cout << "File " << selection_ << " has been opened" << endl;
  else{
    cout << "File " << selection_ << " cannot be found" << endl;
    return;
  }
  
  while (is.good()){
    is.getline(c,999);     
    if (is.good()){
      string line(c);
      cout << line << endl;

      if(line.find("$")!=string::npos){

	size_t lastDollar   = line.rfind("$") +1;
	size_t lastSlash    = line.rfind("/") -1;
	size_t backSlash    = line.rfind("\\")-1;
	
	string LatexYield = line.substr(0,lastDollar);
	string Yield      = line.substr(1,backSlash);
	string Error      = LatexYield.substr(LatexYield.find("m")+1,string::npos);
	Error.erase(Error.size()-1);
	string Name       = line.substr(lastSlash+2,string::npos);
	
	cout << "latex yield " << LatexYield << ", " << Yield << ", " << Error << ", " << Name << endl;
	lMapInclusive_[Name] = LatexYield;

	if(Name.find("TTbar")!=string::npos || 
	   ((Name.find("SS")!=string::npos      && (selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos)) ||
	    (Name.find("AntiIso")!=string::npos && (selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)) ||
	    (Name.find("AntiIso")!=string::npos &&  selection_.find("boost")!=string::npos)
	    ) || 
	   Name.find("WJets")!=string::npos || 
	   Name.find("DYToTauTau")!=string::npos || 
	   Name.find("DYEtoTau")!=string::npos || 
	   Name.find("DYMutoTau")!=string::npos || 
	   Name.find("DYJtoTau")!=string::npos || 
	   Name.find("Other")!=string::npos
	   ){
	  yield += atof(Form("%s",Yield.c_str()));
	  if(Error.find("nan")==string::npos) 
	    error += atof(Form("%s",Error.c_str()))*atof(Form("%s",Error.c_str()));
	}

	if(Name.find("ggH115")!=string::npos || 
	   Name.find("qqH115")!=string::npos || 
	   Name.find("VH115")!=string::npos){
	  signal       += atof(Form("%s",Yield.c_str()));
	  if(Error.find("nan")==string::npos) 
	    signalError  += (atof(Form("%s",Error.c_str()))*atof(Form("%s",Error.c_str())));
	}

      }

    }
  }
  lMapInclusive_["Sum"]    = string(Form("$%.0f\\pm%.0f$",yield, TMath::Sqrt(error)));
  lMapInclusive_["Signal"] = string(Form("$%.1f\\pm%.1f$",signal,TMath::Sqrt(signalError)));
  is.close();

  delete c;
  return;
}



void makeTables( TString stream_ = "MuTau", TString outputDir_   = "Dec2011/iter3"){

  ofstream out(Form("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/Limits/htautau/histograms/%s/yields%s.txt",outputDir_.Data(),stream_.Data()),ios_base::out); 
  out.precision(5);

  // Inclusive
  std::map<string,string> lMapInclusive;

  // SM0
  std::map<string,string> lMapSM0;

  // SM1
  std::map<string,string> lMapSM1;
  
  // SM2
  std::map<string,string> lMapSM2;

  queryText(stream_,outputDir_,"inclusive",lMapInclusive);
  queryText(stream_,outputDir_,"novbf",    lMapSM0);
  queryText(stream_,outputDir_,"boost",    lMapSM1);
  queryText(stream_,outputDir_,"vbf",      lMapSM2);

  string zfakes = (string(stream_.Data())).find("MuTau")!=string::npos ? 
    "DYMutoTau" : "DYEtoTau";

  out << "\\begin{tabular}[!htbp]{|c|c|c|c|c|} " << endl;
  out << "\\hline" << endl;
  out << "Sample & Inclusive & $0/1$jet & Boosted & VBF \\\\" << endl;
  out << "\\hline" << endl;
  out << "Z\\to\\tau\\tau"       << " & " <<   lMapInclusive["DYToTauTau"] << " & " << lMapSM0["DYToTauTau"] <<  " & " << lMapSM1["DYToTauTau"] << " & " << lMapSM2["DYToTauTau"] << "  \\\\ " << endl;
  out << "\\hline" << endl;
  out << "Z\\to ll,l\\to\\tau"   << " & " <<   lMapInclusive[zfakes]       << " & " << lMapSM0[zfakes]       <<  " & " << lMapSM1[zfakes]       << " & " << lMapSM2[zfakes] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "Z\\to ll,jet\\to\\tau" << " & " <<   lMapInclusive["DYJtoTau"]   << " & " << lMapSM0["DYJtoTau"] <<    " & " << lMapSM1["DYJtoTau"]   << " & " << lMapSM2["DYJtoTau"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "W+jets"                << " & " <<   lMapInclusive["WJets"]      << " & " << lMapSM0["WJets"] <<       " & " << lMapSM1["WJets"]      << " & " << lMapSM2["WJets"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "QCD"                   << " & " <<   lMapInclusive["SS"]         << " & " << lMapSM0["SS"] <<          " & " << lMapSM1["AntiIso"]    << " & " << lMapSM2["AntiIso"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "Top"                   << " & " <<   lMapInclusive["TTbar"]      << " & " << lMapSM0["TTbar"] <<       " & " << lMapSM1["TTbar"]      << " & " << lMapSM2["TTbar"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "Di-boson"              << " & " <<   lMapInclusive["Others"]     << " & " << lMapSM0["Others"] <<      " & " << lMapSM1["Others"]     << " & " << lMapSM2["Others"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "\\hline" << endl;
  out << "\\Sum SM"              << " & " <<   lMapInclusive["Sum"]        << " & " << lMapSM0["Sum"] <<         " & " << lMapSM1["Sum"]        << " & " << lMapSM2["Sum"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "Data"                  << " & " <<   lMapInclusive["Data"]       << " & " << lMapSM0["Data"] <<        " & " << lMapSM1["Data"]       << " & " << lMapSM2["Data"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out << "\\hline" << endl;
  out << "Higgs(120)"            << " & " <<   lMapInclusive["Signal"]     << " & " << lMapSM0["Signal"] <<      " & " << lMapSM1["Signal"]     << " & " << lMapSM2["Signal"] << " \\\\  " << endl;
  out << "\\hline" << endl;
  out<<"\\end{tabular}"<<endl;

  out.close();

}


int main(int argc, const char* argv[])
{

  std::cout << "makeTables()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  makeTables();

}
