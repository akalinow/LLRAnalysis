
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <assert.h>

//-------------------------------------------------------------------------------
// CV: code copied from LLRAnalysis/HadTauStudies/bin/prepareTauTauDatacards.cc
TDirectory* getDirectory(TFile* inputFile, const std::string& region, const std::string& category, const std::string& tauPtBin, bool enableException)
{
  std::string dirName = "";
  //if ( tauPtBin == "tau1PtGt45tau2PtGt45" ) dirName = Form("tauTau_%s_%s", region.data(), category.data());
  //else dirName = Form("tauTau_%s_%s/%s", region.data(), category.data(), tauPtBin.data());
  dirName = Form("tauTau_%s_%s/%s", region.data(), category.data(), tauPtBin.data());
  std::cout << "loading directory = " << dirName << " from file = " << inputFile->GetName() << std::endl;
  TDirectory* dir = dynamic_cast<TDirectory*>(inputFile->Get(dirName.data()));
  if ( enableException && !dir ) {
    std::cerr << "<getDirectory>: Failed to find directory = " << dirName << " in file = " << inputFile->GetName() << " !!" << std::endl;
    std::cerr << " (category = " << category << ", tauPtBin = " << tauPtBin << ")" << std::endl;
    assert(0);
  }
  return dir;
}

TH1* getHistogram(TDirectory* dir, const std::string& process, const std::string& histogramName, const std::string& central_or_shift, bool enableException)
{
  std::string histogramName_full = Form("%s/%s", process.data(), process.data());
  if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_full.append("_").append(central_or_shift);
  histogramName_full.append("_").append(histogramName);
  TH1* histogram = dynamic_cast<TH1*>(dir->Get(histogramName_full.data()));
  if ( enableException && !histogram ) {
    std::cerr << "<getHistogram>: Failed to find histogram = " << histogramName_full << " in directory = " << dir->GetName() << " !!" << std::endl;
    assert(0);
  }
  return histogram;
}
//-------------------------------------------------------------------------------

TH1* copyHistogram(TFile* inputFile, const std::string& region_input, const std::string& process_input, const std::string& tauEtaBin_input, const std::string& histogramName_input, 
		   const std::string& category_input, const std::string& tauPtBin_input,
		   TFile* outputFile, const std::string& region_output, const std::string& process_output, const std::string& tauEtaBin_output, const std::string& histogramName_output,
		   const std::string& category_output)
{
  assert(inputFile);
  TDirectory* dir_input = getDirectory(inputFile, region_input, category_input, tauPtBin_input, true);
  assert(dir_input);

  std::string histogramName_full = histogramName_input.data();
  if ( tauEtaBin_input != "" ) histogramName_full.append("_").append(tauEtaBin_input);
  TH1* histogram_input = getHistogram(dir_input, process_input, histogramName_full, "", true);
  assert(histogram_input);

  assert(outputFile);
  outputFile->cd();
  std::string dirName_output = Form("%s_%s_%s", region_output.data(), category_output.data(), tauEtaBin_output.data());
  TDirectory* dir_output = dynamic_cast<TDirectory*>(outputFile->Get(dirName_output.data()));
  if ( !dir_output ) dir_output = outputFile->mkdir(dirName_output.data());
  assert(dir_output);
  dir_output->cd();
  TH1* histogram_output = (TH1*)histogram_input->Clone(Form("%s_%s", process_output.data(), histogramName_output.data()));
  histogram_output->Write();
  return histogram_output;
}

TF1* copyFitFunction_tau(TFile* inputFile, const std::string& region_input, const std::string& tauEtaBin_input,
			 TFile* outputFile, const std::string& region_output, const std::string& tauEtaBin_output)
{
  std::cout << "<copyFitFunction_tau>: region = " << region_input << ", etaBin = " << tauEtaBin_input << std::endl;
  assert(inputFile);
  TDirectory* dir_input = dynamic_cast<TDirectory*>(inputFile->Get(Form("jetToTauFakeRate/inclusive/%s", tauEtaBin_input.data())));
  assert(dir_input);

  TF1* fitFunction_norm_input = dynamic_cast<TF1*>(dir_input->Get(Form("fitFunctionNorm_%s", region_input.data())));
  assert(fitFunction_norm_input);
  double norm = fitFunction_norm_input->Eval(100.);

  TF1* fitFunction_shape_input = dynamic_cast<TF1*>(dir_input->Get(Form("fitFunctionShape_tau1PtS_%s", region_input.data())));
  assert(fitFunction_shape_input);

  assert(outputFile);
  outputFile->cd();
  std::string dirName_output = Form("%s_%s", region_output.data(), tauEtaBin_output.data());
  TDirectory* dir_output = dynamic_cast<TDirectory*>(outputFile->Get(dirName_output.data()));
  if ( !dir_output ) dir_output = outputFile->mkdir(dirName_output.data());
  assert(dir_output);
  dir_output->cd();
  TF1* fitFunction_output = (TF1*)fitFunction_shape_input->Clone("fitFunction");
  // CV: fitFunction is linear, so simple multiplication of fit parameters for shape and norm is sufficient here...
  assert(fitFunction_output->GetNpar() == 2);
  std::cout << "fitParameter:" << std::endl;
  std::cout << " norm = " << norm << std::endl;
  double shape_offset = fitFunction_shape_input->GetParameter(0);
  double shape_slope = fitFunction_shape_input->GetParameter(1);
  std::cout << " shape: offset = " << shape_offset << ", slope = " << shape_slope << std::endl;
  fitFunction_output->SetParameter(0, norm*shape_offset);
  fitFunction_output->SetParameter(1, norm*shape_slope);
  fitFunction_output->Write();
  return fitFunction_output;
}

TF1* copyFitFunction_bjet(TFile* inputFile, const std::string& region_input, const std::string& tauEtaBin_input,
			  TFile* outputFile, const std::string& region_output, const std::string& tauEtaBin_output)
{
  std::cout << "<copyFitFunction_bjet>: region = " << region_input << ", etaBin = " << tauEtaBin_input << std::endl;
  assert(inputFile);
  TDirectory* dir_input = dynamic_cast<TDirectory*>(inputFile->Get(Form("bJetLooseToTightWeight/btag/%s", tauEtaBin_input.data())));
  assert(dir_input);

  TF1* fitFunction_norm_input = dynamic_cast<TF1*>(dir_input->Get(Form("fitFunctionNorm_%s", region_input.data())));
  assert(fitFunction_norm_input);
  double norm = fitFunction_norm_input->Eval(100.);

  TF1* fitFunction_shape_input = dynamic_cast<TF1*>(dir_input->Get(Form("fitFunctionShape_bJet1PtS_%s", region_input.data())));
  assert(fitFunction_shape_input);

  assert(outputFile);
  outputFile->cd();
  std::string dirName_output = Form("%s_%s", region_output.data(), tauEtaBin_output.data());
  TDirectory* dir_output = dynamic_cast<TDirectory*>(outputFile->Get(dirName_output.data()));
  if ( !dir_output ) dir_output = outputFile->mkdir(dirName_output.data());
  assert(dir_output);
  dir_output->cd();
  TF1* fitFunction_output = (TF1*)fitFunction_shape_input->Clone("fitFunction");
  if ( fitFunction_output->GetNpar() == 2 ) {
    // CV: fitFunction is linear, so simple multiplication of fit parameters for shape and norm is sufficient here...
    assert(fitFunction_output->GetNpar() == 2);
    std::cout << "fitParameter:" << std::endl;
    std::cout << " norm = " << norm << std::endl;
    double shape_offset = fitFunction_shape_input->GetParameter(0);
    double shape_slope = fitFunction_shape_input->GetParameter(1);
    std::cout << " shape: offset = " << shape_offset << ", slope = " << shape_slope << std::endl;
    fitFunction_output->SetParameter(0, norm*shape_offset);
    fitFunction_output->SetParameter(1, norm*shape_slope);
  } else if ( fitFunction_output->GetNpar() == 3 ) {
    assert(fitFunction_output->GetNpar() == 3);
    std::cout << "fitParameter:" << std::endl;
    std::cout << " norm = " << norm << std::endl;
    double shape_norm = fitFunction_shape_input->GetParameter(0);
    double shape_slope = fitFunction_shape_input->GetParameter(1);
    double shape_offset = fitFunction_shape_input->GetParameter(2);
    std::cout << " shape: norm = " << shape_norm << ", slope = " << shape_slope << ", offset = " << shape_offset << std::endl;
    fitFunction_output->SetParameter(0, TMath::Sqrt(norm)*shape_norm);
    fitFunction_output->SetParameter(1, shape_slope);
    fitFunction_output->SetParameter(2, shape_offset);
  } else assert(0);
  fitFunction_output->Write();
  return fitFunction_output;
}

void extractShapeTemplatesQCDforRam()
{
  gROOT->SetBatch(true);

  TH1::AddDirectory(false);

  std::string inputFilePath_histograms = "/data1/veelken/tmp/tauTauAnalysis/v3_01_3/";
  std::string inputFileName_histograms = "MVAwLToldDMsTight/hadd_stage5_MVAwLToldDMsTight.root";
  TFile* inputFile_histograms = new TFile(Form("%s%s", inputFilePath_histograms.data(), inputFileName_histograms.data()));
  if ( !inputFile_histograms ) {
    std::cerr << "Failed to open input file containing histograms !!" << std::endl;
    assert(0);
  }

  std::string inputFilePath_fitFunctions_tau = "/data1/veelken/tmp/tauTauAnalysis/v3_01_3/";
  std::string inputFileName_fitFunctions_tau = "MVAwLToldDMsTight/determineJetToTauFakeRate_MVAwLToldDMsTight.root";
  TFile* inputFile_fitFunctions_tau = new TFile(Form("%s%s", inputFilePath_fitFunctions_tau.data(), inputFileName_fitFunctions_tau.data()));
  if ( !inputFile_fitFunctions_tau ) {
    std::cerr << "Failed to open input file containing fitFunctions !!" << std::endl;
    assert(0);
  }
  std::string inputFilePath_fitFunctions_bjet = "/data1/veelken/tmp/tauTauAnalysis/v3_01_3/";
  std::string inputFileName_fitFunctions_bjet = "MVAwLToldDMsTight/determineBJetLooseToTightWeight_MVAwLToldDMsTight.root";
  TFile* inputFile_fitFunctions_bjet = new TFile(Form("%s%s", inputFilePath_fitFunctions_bjet.data(), inputFileName_fitFunctions_bjet.data()));
  if ( !inputFile_fitFunctions_bjet ) {
    std::cerr << "Failed to open input file containing fitFunctions !!" << std::endl;
    assert(0);
  }

  std::vector<std::string> regions_input;
  regions_input.push_back("SSiso1_iso2_LooseBtag");
  regions_input.push_back("SSrelaxed1_iso2_LooseBtag");
  regions_input.push_back("SSvrelaxed1_iso2_LooseBtag");  
  regions_input.push_back("SSvrelaxed1_vrelaxed2_LooseBtag");  
   regions_input.push_back("SSvrelaxed1_vrelaxed2_TightBtag");  
  //regions_input.push_back("OSrelaxed1FRw2ndTauTight_iso2_TightBtag");
  //regions_input.push_back("OSvrelaxed1FRw2ndTauTight_iso2_TightBtag");

  std::vector<std::string> regions2_input;
  regions2_input.push_back("OSrelaxed1_iso2_TightBtag");
  regions2_input.push_back("OSvrelaxed1_iso2_LooseBtag");

  std::map<std::string, std::string> regions_output; // key = region_input
  regions_output["SSiso1_iso2_LooseBtag"] = "SSiso";
  regions_output["SSrelaxed1_iso2_LooseBtag"] = "SSloose";
  regions_output["SSvrelaxed1_iso2_LooseBtag"] = "SSvloose";  
  regions_output["SSvrelaxed1_vrelaxed2_LooseBtag"] = "SSvloose_LooseBtag"; 
  regions_output["SSvrelaxed1_vrelaxed2_TightBtag"] = "SSvloose_TightBtag";
  //regions_output["OSrelaxed1FRw2ndTauTight_iso2_TightBtag"] = "OSlooseFRw";
  //regions_output["OSvrelaxed1FRw2ndTauTight_iso2_LooseBtagFRw"] = "OSvlooseFRw";

  std::map<std::string, std::string> regions2_output; // key = region2_input
  regions2_output["OSrelaxed1_iso2_TightBtag"] = "OSloose";
  regions2_output["OSvrelaxed1_iso2_LooseBtag"] = "OSvloose";

  std::vector<std::string> processes_input;
  processes_input.push_back("data_obs");
  processes_input.push_back("ZTTmc");
  processes_input.push_back("ZL");
  processes_input.push_back("ZJ");
  processes_input.push_back("W");
  processes_input.push_back("TT");
  processes_input.push_back("VV");

  std::map<std::string, std::string> processes_output; // key = process_input
  processes_output["data_obs"] = "data_obs";
  processes_output["ZTTmc"] = "ZTT";
  processes_output["ZL"] = "ZL";
  processes_output["ZJ"] = "ZJ";
  processes_output["W"] = "W";
  processes_output["TT"] = "TT";
  processes_output["VV"] = "VV";

  std::vector<std::string> histograms_input;
  histograms_input.push_back("tau1PtS");
  histograms_input.push_back("bJet1PtS");

  std::map<std::string, std::string> histograms_output; // key = histogram_input
  histograms_output["tau1PtS"] = "leadTauPt";
  histograms_output["bJet1PtS"] = "bjetPt";

  std::string outputFileName = "extractShapeTemplatesQCDforRam.root";
  TFile* outputFile = new TFile(outputFileName.data(), "RECREATE");

  // CV: copy histograms
  for ( std::vector<std::string>::const_iterator region_input = regions_input.begin();
	region_input != regions_input.end(); ++region_input ) {
    std::string region_output = regions_output[*region_input];
    assert(region_output != "");
    for ( std::vector<std::string>::const_iterator process_input = processes_input.begin();
	  process_input != processes_input.end(); ++process_input ) {
      std::string process_input_tmp = (*process_input);
      if ( (*process_input) == "W" ) process_input_tmp = "Wtmp";
      std::string process_output = processes_output[*process_input];
      assert(process_output != "");
      for ( std::vector<std::string>::const_iterator histogram_input = histograms_input.begin();
	    histogram_input != histograms_input.end(); ++histogram_input ) {

	if ( (*histogram_input) == "tau1PtS" ) {
	  if ( (*region_input) == "SSvrelaxed1_iso2_LooseBtag" || (*region_input) == "SSvrelaxed1_iso2_TightBtag" ) continue;
	  
	  std::string histogram_output = histograms_output[*histogram_input];
	  assert(histogram_output != "");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "tau1EtaLt12",   *histogram_input,
			"inclusive", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEtaLt12", histogram_output,
			"inclusive");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "tau1Eta12to17", *histogram_input, 
			"inclusive", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEta12to17", histogram_output,
			"inclusive");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "tau1EtaGt17",   *histogram_input, 
			"inclusive", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEtaGt17", histogram_output,
			"inclusive");
	}
	  
	if ( (*histogram_input) == "bJet1PtS" ) {
	  if ( !((*region_input) == "SSvrelaxed1_vrelaxed2_LooseBtag" || (*region_input) == "SSvrelaxed1_vrelaxed2_TightBtag") ) continue;
	  
	  std::string histogram_output = histograms_output[*histogram_input];
	  assert(histogram_output != "");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "bJet1EtaLt10",   *histogram_input,
			"btag", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEtaLt10", histogram_output,
			"btag");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "bJet1Eta10to20", *histogram_input, 
			"btag", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEta10to20", histogram_output,
			"btag");
	  copyHistogram(inputFile_histograms, *region_input, process_input_tmp,  "bJet1EtaGt20",   *histogram_input, 
			"btag", "tau1PtGt45tau2PtGt45",
			outputFile, region_output, process_output, "absEtaGt20", histogram_output,
			"btag");
	}
      }      
    }
  }

  for ( std::vector<std::string>::const_iterator region_input = regions2_input.begin();
	region_input != regions2_input.end(); ++region_input ) {
    std::string region_output = regions2_output[*region_input];
    assert(region_output != "");
    for ( std::vector<std::string>::const_iterator process_input = processes_input.begin();
	  process_input != processes_input.end(); ++process_input ) {
      std::string process_input_tmp = (*process_input);
      if ( (*process_input) == "W" ) process_input_tmp = "Wtmp";
      std::string process_output = processes_output[*process_input];
      assert(process_output != "");
      for ( std::vector<std::string>::const_iterator histogram_input = histograms_input.begin();
	    histogram_input != histograms_input.end(); ++histogram_input ) {
	std::string histogram_output = histograms_output[*histogram_input];
	assert(histogram_output != "");
	copyHistogram(inputFile_histograms, *region_input, process_input_tmp, "", *histogram_input, 
		      "nobtag", "tau1PtGt45tau2Pt45to60",
		      outputFile, region_output, process_output, "", histogram_output,
		      "nobtag_low");
	copyHistogram(inputFile_histograms, *region_input, process_input_tmp, "", *histogram_input, 
		      "nobtag", "tau1PtGt45tau2Pt60to80",
		      outputFile, region_output, process_output, "", histogram_output,
		      "nobtag_medium");
	copyHistogram(inputFile_histograms, *region_input, process_input_tmp, "", *histogram_input, 
		      "nobtag", "tau1PtGt45tau2PtGt80",
		      outputFile, region_output, process_output, "", histogram_output,
		      "nobtag_high");
	copyHistogram(inputFile_histograms, *region_input, process_input_tmp, "", *histogram_input, 
		      "btag", "tau1PtGt45tau2Pt45to60",
		      outputFile, region_output, process_output, "", histogram_output,
		      "btag_low");
	copyHistogram(inputFile_histograms, *region_input, process_input_tmp, "", *histogram_input, 
		      "btag", "tau1PtGt45tau2PtGt60",
		      outputFile, region_output, process_output, "", histogram_output,
		      "btag_high");
      }      
    }
  }

  // CV: copy fitFunctions
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag",  "tau1EtaLt12",
		      outputFile,                 "SStightTau_div_looseTau",                              "absEtaLt12");
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag",  "tau1Eta12to17",
		      outputFile,                 "SStightTau_div_looseTau",                              "absEta12to17");
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag",  "tau1EtaGt17",
		      outputFile,                 "SStightTau_div_looseTau",                              "absEtaGt17");	
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag", "tau1EtaLt12",
		      outputFile,                 "SStightTau_div_vlooseTau",                             "absEtaLt12");
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag", "tau1Eta12to17",
		      outputFile,                 "SStightTau_div_vlooseTau",                             "absEta12to17");
  copyFitFunction_tau(inputFile_fitFunctions_tau, "SSiso1_iso2_LooseBtag_div_SSvrelaxed1_iso2_LooseBtag", "tau1EtaGt17",
		      outputFile,                 "SStightTau_div_vlooseTau",                             "absEtaGt17");	

  copyFitFunction_bjet(inputFile_fitFunctions_bjet, "SSvrelaxed1_vrelaxed2_TightBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag", "bJet1EtaLt10",
		       outputFile,                  "SStightBtag_div_looseBtag",                                           "absEtaLt10");
  copyFitFunction_bjet(inputFile_fitFunctions_bjet, "SSvrelaxed1_vrelaxed2_TightBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag", "bJet1Eta10to20",
		       outputFile,                  "SStightBtag_div_looseBtag",                                           "absEta10to20");
  copyFitFunction_bjet(inputFile_fitFunctions_bjet, "SSvrelaxed1_vrelaxed2_TightBtag_div_SSvrelaxed1_vrelaxed2_LooseBtag", "bJet1EtaGt20",
		       outputFile,                  "SStightBtag_div_looseBtag",                                           "absEtaGt20");
  
  delete outputFile;

  delete inputFile_histograms;

  delete inputFile_fitFunctions_tau;
  delete inputFile_fitFunctions_bjet;
}
