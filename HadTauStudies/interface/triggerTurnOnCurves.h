#ifndef LLRAnalysis_HadTauStudies_triggerTurnOnCurves_h
#define LLRAnalysis_HadTauStudies_triggerTurnOnCurves_h

double eff2012IsoParkedTau19fb_Simone(double, double);
double eff2012IsoParkedTau19fbMC_Simone(double, double);
double eff2012IsoTau19fb_Simone(double, double);
double eff2012IsoTau19fbMC_Simone(double, double);
double eff2012Jet19fb(double, double);

double effDrop2012IsoParkedTau_Arun_cutMedium(double, double);
double eff2012IsoParkedTau_Arun_cutMedium(double, double, bool);
double effDrop2012IsoParkedTau_Arun_mvaTight(double, double);
double eff2012IsoParkedTau_Arun_mvaTight(double, double, bool);
double effDrop2012IsoParkedTau_Arun_mvaVTight(double, double);
double eff2012IsoParkedTau_Arun_mvaVTight(double, double, bool);
double eff2012IsoParkedTauMC_Arun_cutMedium(double, double, bool);
double eff2012IsoParkedTauMC_Arun_mvaTight(double, double, bool);
double eff2012IsoParkedTauMC_Arun_mvaVTight(double, double, bool);

double effPFJet320_vs_jetPt(double, double);
double effPFJet320MC_vs_jetPt(double, double);
double effPFJet320MC_vs_tauPt(double, double);
double effPFJet320(double, double, double);
double effPFJet320MC(double, double, double);

#endif
