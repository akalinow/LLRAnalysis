#ifndef LLRAnalysis_HadTauStudies_particleIDlooseToTightWeightEntryType_h
#define LLRAnalysis_HadTauStudies_particleIDlooseToTightWeightEntryType_h

/** \class particleIDlooseToTightWeightEntryType
 *
 * Auxiliary class to apply jet -> tau fake-rates and mistag rates for b-jets
 *
 * \author Christian Veelken, LLR
 *
 */

#include <TFile.h>
#include <TMath.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>

#include <string>
#include <vector>
#include <assert.h>

struct particleIDlooseToTightWeightEntryType
{
  particleIDlooseToTightWeightEntryType(TFile*, const std::string&, double, double, double, double,
					const std::string&, 
					const std::string&, const std::string&, const std::string&, bool, double, 
					const std::string&, const std::string&, const std::string&, bool, double);
  ~particleIDlooseToTightWeightEntryType();
  double weight(double particle1Pt, double particle2Pt) const;
  double weightErr_relative(double particle1Pt, double particle2Pt) const;
  double particle1EtaMin_;
  double particle1EtaMax_;
  double particle2EtaMin_;
  double particle2EtaMax_;
  enum { kNotApplied, kFitFunction, kGraph };
  TF1* norm_;
  TGraphAsymmErrors* graphShapeCorr_particle1_;
  TGraphAsymmErrors* graphShapeCorrErrUp_particle1_;
  TGraphAsymmErrors* graphShapeCorrErrDown_particle1_;
  TF1* fitFunctionShapeCorr_particle1_central_;
  TF1* fitFunctionShapeCorr_particle1_shift_;
  double shapeCorrPow_particle1_;
  bool applyFitFunction_or_graph_tau1_; 
  TGraphAsymmErrors* graphShapeCorr_particle2_;
  TGraphAsymmErrors* graphShapeCorrErrUp_particle2_;
  TGraphAsymmErrors* graphShapeCorrErrDown_particle2_;
  TF1* fitFunctionShapeCorr_particle2_central_;
  TF1* fitFunctionShapeCorr_particle2_shift_;
  double shapeCorrPow_particle2_;
  bool applyFitFunction_or_graph_tau2_; 
};

#endif

