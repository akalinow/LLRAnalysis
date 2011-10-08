#include <cstdlib>
#include <iostream>
#include <fstream> 
#include <map>
#include <string>

#include "TChain.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"


#define VERBOSE true

using namespace std;



void cutFlowStudyMu( ) 
{

  int mH = 120;

  ofstream out("cutFlowMuTau.txt"); 
  out.precision(6);

  float Lumi = (159.+887.+361.7+531.5)*1.00;

  TFile *fBackgroundDY
    = new TFile("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/TauTauStudies/test/Macro/skimLog/MuTauStream/DYJets/skimMuTauStream.root","READ"); 
  TFile *fBackgroundW   
    = new TFile("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/TauTauStudies/test/Macro/skimLog/MuTauStream/W/skimMuTauStream.root","READ"); 
  TFile *fBackgroundTT  
    = new TFile("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/TauTauStudies/test/Macro/skimLog/MuTauStream/TT/skimMuTauStream.root","READ"); 
  TFile *fBackgroundVBF120  
    = new TFile("/home/llr/cms/lbianchini/CMSSW_4_2_5/src/LLRAnalysis/TauTauStudies/test/Macro/skimLog/MuTauStream/VBF120/skimMuTauStream.root","READ"); 

  std::map<std::string,TFile*> fullMap;
  fullMap["DY"]   = fBackgroundDY;
  fullMap["W"]    = fBackgroundW;
  fullMap["TT"]   = fBackgroundTT;
  fullMap["VBF120"]   = fBackgroundVBF120;

  std::vector<string> samples;
  samples.push_back("DY");
  samples.push_back("W");
  samples.push_back("TT");
  samples.push_back("VBF120");

  TFile *fBackgroundDYSkim
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSummer11_fAna//nTupleDYJets-Mu-50-madgraph-PUS4_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundWSkim   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSummer11_fAna//nTupleWJets-Mu-madgraph-PUS4_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundTTSkim  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSummer11_fAna//nTupleTTJets-Mu-madgraph-PUS4_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundVBF120Skim  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSummer11_fAna//nTupleVBFH120-Mu-powheg-PUS4_run_Open_MuTauStream.root","READ"); 

  TString tree = "outTreePtOrd";

  TTree *DYtree     = (TTree*)fBackgroundDYSkim->Get(tree);
  TTree *Wtree      = (TTree*)fBackgroundWSkim->Get(tree);
  TTree *TTtree     = (TTree*)fBackgroundTTSkim->Get(tree);
  TTree *VBF120tree = (TTree*)fBackgroundVBF120Skim->Get(tree);

  std::map<std::string,TTree*>::iterator jt;

  TFile *fBackgroundDYSkimFull
    = new TFile("/data_CMS/cms/lbianchini/MuTauStreamSummer11_fAna/treeMuTauStream_DYJets-Mu-50-madgraph-PUS4_run.root","READ"); 
  TFile *fBackgroundWSkimFull   
    = new TFile("/data_CMS/cms/lbianchini/MuTauStreamSummer11_fAna/treeMuTauStream_WJets-Mu-madgraph-PUS4_run.root","READ"); 
  TFile *fBackgroundTTSkimFull  
    = new TFile("/data_CMS/cms/lbianchini/MuTauStreamSummer11_fAna/treeMuTauStream_TTJets-Mu-madgraph-PUS4_run.root","READ"); 
  TFile *fBackgroundVBF120SkimFull  
    = new TFile("/data_CMS/cms/lbianchini/MuTauStreamSummer11_fAna/treeMuTauStream_VBFH120-Mu-powheg-PUS4_run.root","READ"); 

  std::map<std::string,TFile*> skimMap;
  skimMap["DY"]     = fBackgroundDYSkimFull;
  skimMap["W"]      = fBackgroundWSkimFull;
  skimMap["TT"]     = fBackgroundTTSkimFull;
  skimMap["VBF120"] = fBackgroundVBF120SkimFull;

  std::map<std::string,TTree*> tMap;
  tMap["DY"] = DYtree;
  tMap["W"]  = Wtree;
  tMap["TT"] = TTtree;
  tMap["VBF120"] = VBF120tree;

  cout << "trees ready" << endl;

  std::vector<string> filters;
  filters.push_back("total");
  filters.push_back("vertex");
  filters.push_back("at least 1 mu-tau");
  filters.push_back("mu pt-eta");
  filters.push_back("mu-ID");
  filters.push_back("tau pt-eta");
  filters.push_back("tau-ID");
  filters.push_back("tau against-mu");
  filters.push_back("tau against-e");
  filters.push_back("tau-iso and mu-iso");
  filters.push_back("di-mu veto");
  filters.push_back("pZeta");
  filters.push_back("OS");

  std::map<std::string,TFile*>::iterator it;

  std::map<std::string,float> cutMap_allEventsFilter;
  std::map<std::string,float> cutMap_vertexScrapingFilter;
  std::map<std::string,float> cutMap_atLeastOneMuTauFilter;
  std::map<std::string,float> cutMap_muPtEtaFilter;
  std::map<std::string,float> cutMap_muPtEtaIDFilter;
  std::map<std::string,float> cutMap_tauPtEtaFilter;
  std::map<std::string,float> cutMap_tauPtEtaIDFilter;
  std::map<std::string,float> cutMap_tauPtEtaIDAgMuFilter;
  std::map<std::string,float> cutMap_tauPtEtaIDAgMuAgElecFilter;
  std::map<std::string,float> cutMap_TauMuIso;
  std::map<std::string,float> cutMap_DiMuVeto;
  std::map<std::string,float> cutMap_Mt;
  std::map<std::string,float> cutMap_OS;

  std::vector< std::map<std::string,float> > allFilters;
  allFilters.push_back(cutMap_allEventsFilter);
  allFilters.push_back(cutMap_vertexScrapingFilter);
  allFilters.push_back(cutMap_atLeastOneMuTauFilter);
  allFilters.push_back(cutMap_muPtEtaFilter);
  allFilters.push_back(cutMap_muPtEtaIDFilter);
  allFilters.push_back(cutMap_tauPtEtaFilter);
  allFilters.push_back(cutMap_tauPtEtaIDFilter);
  allFilters.push_back(cutMap_tauPtEtaIDAgMuFilter);
  allFilters.push_back(cutMap_tauPtEtaIDAgMuAgElecFilter);

  std::vector< string > filtersByName;
  filtersByName.push_back("allEventsFilter");
  filtersByName.push_back("vertexScrapingFilter");
  filtersByName.push_back("atLeastOneMuTauFilter");
  filtersByName.push_back("muPtEtaFilter");
  filtersByName.push_back("muPtEtaIDFilter");
  filtersByName.push_back("tauPtEtaFilter");
  filtersByName.push_back("tauPtEtaIDFilter");
  filtersByName.push_back("tauPtEtaIDAgMuFilter");
  filtersByName.push_back("tauPtEtaIDAgMuAgElecFilter");

  std::vector< std::map<std::string,float> > offlineFilters;
  offlineFilters.push_back(cutMap_TauMuIso);
  offlineFilters.push_back(cutMap_DiMuVeto);
  offlineFilters.push_back(cutMap_Mt);
  offlineFilters.push_back(cutMap_OS);

  std::vector< string > offlineCuts;
  offlineCuts.push_back("(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10)");
  offlineCuts.push_back("muFlag==0");
  offlineCuts.push_back("(pZetaCorr-1.5*pZetaVisCorr>-20)");
  offlineCuts.push_back("diTauCharge==0");
 
  for(unsigned int k = 0 ; k <filtersByName.size() ; k++){
    
    for(it = fullMap.begin(); it != fullMap.end(); it++){
      TH1F* allEvents = (TH1F*)(it->second)->Get( (filtersByName[k]+"/totalEvents").c_str() );
      float tot =  allEvents->GetBinContent(1);
      (allFilters[k])[it->first]  = tot;
    }
  }

  std::map<std::string,float> helper;
  helper["DY"] = 0.;
  helper["W"]  = 0.;
  helper["TT"] = 0.;
  helper["VBF120"] = 0.;

  for(it = skimMap.begin(); it != skimMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    TTree* tree = tMap[ it->first ];
    TH1F* h1 = new TH1F("h1","",1,-10,10);
    tree->Draw("etaL1>>h1");
    float cut = h1->Integral();
    helper[it->first]  = cut/tot;
  }


  for(unsigned int k = 0; k < offlineFilters.size(); k++){    
    for(jt = tMap.begin(); jt != tMap.end(); jt++){
      cout<<jt->first<<endl;
      TH1F* h1 = new TH1F("h1","",1,-10,10);
      string tmp_cut = "ptL1>15 && ptL2>20";
      for(unsigned v = 0; v <=k ; v++){
        tmp_cut = tmp_cut + " && "+offlineCuts[v];
      }
      tmp_cut = "("+tmp_cut+")";
      cout << tmp_cut << endl;
      TCut cut(tmp_cut.c_str());

      jt->second->Draw("etaL1>>h1",cut);

      float tot = h1->Integral();
      cout << tot << endl;
      (offlineFilters[k])[jt->first] = tot;
      delete h1;
    }
  }

  for(unsigned k = 0; k<offlineFilters.size() ; k++) allFilters.push_back(offlineFilters[k]);

  out<<"\\begin{tabular}[!htbp]{|c";
  for(int k = 0 ; k < samples.size(); k++) out<<"|c";
  out<<"|c|} \\hline"<<endl;
  out<< "Cut & ";
  for(int k = 0 ; k < samples.size(); k++){
    out << (fullMap.find(samples[k]))->first;
    if(k!=samples.size()-1) out <<" & " ;
    else out << " \\\\ " << endl;
  }
  out <<  " \\hline" << endl;

  for(int i = 0; i < allFilters.size(); i++){
    out << filters[i] << " & ";
    for(int k = 0 ; k < samples.size(); k++){

      if(i==0) 
	out << "1.0" ;
      else if(i==9)
	out << helper[samples[k]];
      else
	out << (allFilters[i].find(samples[k]))->second/(allFilters[i-1].find(samples[k]))->second ;

      if(k!=samples.size()-1) out <<" & " ;
      else out << " \\\\ " << endl;
    }
    out <<  " \\hline" << endl;
  }
  
  out<<"\\end{tabular}"<<endl;


}
