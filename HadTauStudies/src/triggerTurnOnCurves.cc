#include "LLRAnalysis/HadTauStudies/interface/triggerTurnOnCurves.h"

#include <TMath.h>

//-----------------------------------------------------------------------------
// CV: copied from https://twiki.cern.ch/twiki/bin/view/CMS/HiggsToTauTauWorkingSummer2013#TauTau_Trigger
  
// Tau Parked with HLT_DoubleMediumIsoPFTau35_Trk*_eta2p1_v*
double eff2012IsoParkedTau19fb_Simone(double pt, double eta) 
{
  return (  0.826969 * 0.5 * (TMath::Erf((pt - 42.2274)/2./0.783258 /sqrt(pt)) + 1.) ) ; // only one eta bin
}

double eff2012IsoParkedTau19fbMC_Simone(double pt, double eta) 
{
  const double data_plateau = 0.826969 ;
  if      ( pt < 140. ) return ( 0.813769 * 0.5 * (TMath::Erf((pt - 39.9322)/2./0.819354 /sqrt(pt)) + 1.) ) ; // only one eta bin
  else if ( pt > 400. ) return data_plateau / 2.03467;
  else if ( pt > 300. ) return data_plateau / 1.31593;
  else if ( pt > 250. ) return data_plateau / 1.25698;
  else if ( pt > 200. ) return data_plateau / 1.18941;
  else if ( pt > 180. ) return data_plateau / 1.17448;
  else if ( pt > 160. ) return data_plateau / 1.0964 ;
  else                  return data_plateau / 1.09279;
}

double eff2012IsoTau19fb_Simone(double pt, double eta) 
{
  if ( fabs(eta) < 1.4 ) {
    return (  808.411  * ( 0.764166 * 0.5 * (TMath::Erf((pt - 33.2236)/2./0.97289 /sqrt(pt)) + 1.))   // 2012A by Bastian not split in eta
            + 4428.0   * ( 0.75721  * 0.5 * (TMath::Erf((pt - 39.0836)/2./1.07753 /sqrt(pt)) + 1.))   // 2012B
            + 6892.158 * ( 0.791464 * 0.5 * (TMath::Erf((pt - 38.4932)/2./1.01232 /sqrt(pt)) + 1.))   // 2012C measured in v2 only
            + 7274.    * ( 0.779446 * 0.5 * (TMath::Erf((pt - 38.4603)/2./1.01071 /sqrt(pt)) + 1.)) ) // 2012D measured in one go
          /( 808.411 + 4428.0 + 6892.158 + 7274. );
  } else {
    return (  808.411  * ( 0.764166 * 0.5 * (TMath::Erf((pt - 33.2236)/2./0.97289 /sqrt(pt)) + 1.))   // 2012A by Bastian not split in eta
            + 4428.0   * ( 0.693788 * 0.5 * (TMath::Erf((pt - 37.7719)/2./1.09202 /sqrt(pt)) + 1.))   // 2012B
            + 6892.158 * ( 0.698909 * 0.5 * (TMath::Erf((pt - 36.5533)/2./1.05743 /sqrt(pt)) + 1.))   // 2012C measured in v2 only
            + 7274.    * ( 0.703532 * 0.5 * (TMath::Erf((pt - 38.8609)/2./1.05514 /sqrt(pt)) + 1.)) ) // 2012D measured in one go
          /( 808.411 + 4428.0 + 6892.158 + 7274. );
  } 
}

double eff2012IsoTau19fbMC_Simone(double pt, double eta)
{
  if ( fabs(eta) < 1.4 ) {
    return ( 0.807425 * 0.5 * (TMath::Erf((pt - 35.2214)/2./1.04214  /sqrt(pt)) + 1.) ) ;
  } else {
    return ( 0.713068 * 0.5 * (TMath::Erf((pt - 33.4584)/2./0.994692 /sqrt(pt)) + 1.) ) ;
  }
}

double eff2012Jet19fb(double pt, double eta)
{
  return ( abs(eta) <= 2.1 )*
    ( ( 808.411 * ( 0.99212  * 0.5 * (TMath::Erf((pt - 31.3706)/2./1.22821/sqrt(pt)) + 1.))
     + 4428.0  * ( 0.99059  * 0.5 * (TMath::Erf((pt - 32.1104)/2./1.23292/sqrt(pt)) + 1.))
     + 1783.003* ( 0.988256 * 0.5 * (TMath::Erf((pt - 31.3103)/2./1.18766/sqrt(pt)) + 1.))
     + 5109.155* ( 0.988578 * 0.5 * (TMath::Erf((pt - 31.6391)/2./1.22826/sqrt(pt)) + 1.))
     + 4131.   * ( 0.989049 * 0.5 * (TMath::Erf((pt - 31.9836)/2./1.23871/sqrt(pt)) + 1.))
     + 3143.   * ( 0.988047 * 0.5 * (TMath::Erf((pt - 31.6975)/2./1.25372/sqrt(pt)) + 1.)))
    /(808.411+4428.0+1783.003+5109.155+4131+3143)) +
    ( abs(eta) > 2.1 )*
    ( ( 808.411 *( 0.969591  * 0.5 * (TMath::Erf((pt - 36.8179)/2./0.904254/sqrt(pt)) + 1.))
     + 4428.0  *( 0.975932  * 0.5 * (TMath::Erf((pt - 37.2121)/2./0.961693/sqrt(pt)) + 1.))
     + 1783.003*( 0.990305  * 0.5 * (TMath::Erf((pt - 36.3096)/2./0.979524/sqrt(pt)) + 1.))
     + 5109.155*( 0.971612  * 0.5 * (TMath::Erf((pt - 36.2294)/2./0.871726/sqrt(pt)) + 1.))
     + 4131.   *( 0.977958  * 0.5 * (TMath::Erf((pt - 37.131 )/2./0.987523/sqrt(pt)) + 1.))
     + 3143.   *( 0.968457  * 0.5 * (TMath::Erf((pt - 36.3159)/2./0.895031/sqrt(pt)) + 1.)))
    /(808.411+4428.0+1783.003+5109.155+4131+3143));
}    
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// CV: tau trigger turn-on curves determined by Arun

namespace
{
  double square(double x)
  {
    return (x*x);
  }
}

double effDrop2012IsoParkedTau_Arun_cutMedium(double pt, double eta) // CV: drop of tau trigger efficiency due to bug in PFlow config discovered in Spring 2012 
                                                                     //    (affects Summer'12 MC and 2012 run ABC data, L = 11 fb^-1)
{
  if ( pt < 140. ) return 1.;
  else return ( (1. - 1.09474e-03*(pt - 140.) + 3.53097e-07*square(pt - 140.)) );
}

double eff2012IsoParkedTau_Arun_cutMedium(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.919649 * 0.5 * (TMath::Erf((pt - 43.1776)/2./0.758213 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= ( (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta)) / 18.3 );
  return eff;
}

double effDrop2012IsoParkedTau_Arun_mvaTight(double pt, double eta) // CV: drop of tau trigger efficiency due to bug in PFlow config discovered in Spring 2012 
                                                                    //    (affects Summer'12 MC and 2012 run ABC data, L = 11 fb^-1)
{
  if ( pt < 140. ) return 1.;
  else return ( (1. - 1.10448e-03*(pt - 140.) + 4.09426e-07*square(pt - 140.)) );
}

double eff2012IsoParkedTau_Arun_mvaTight(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.876313 * 0.5 * (TMath::Erf((pt - 42.8216)/2./0.754887 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= ( (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_mvaTight(pt, eta)) / 18.3 );
  return eff;
}

double effDrop2012IsoParkedTau_Arun_mvaVTight(double pt, double eta) // CV: drop of tau trigger efficiency due to bug in PFlow config discovered in Spring 2012 
                                                                     //    (affects Summer'12 MC and 2012 run ABC data, L = 11 fb^-1)
{
  if ( pt < 140. ) return 1.;
  else return ( (1. - 1.09226e-03*(pt - 140.) + 4.00873e-07*square(pt - 140.)) );
}

double eff2012IsoParkedTau_Arun_mvaVTight(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.896198 * 0.5 * (TMath::Erf((pt - 43.0466)/2./0.763838 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= ( (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_mvaTight(pt, eta)) / 18.3 );
  return eff;
}

double eff2012IsoParkedTauMC_Arun_cutMedium(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.880128 * 0.5 * (TMath::Erf((pt - 41.8055)/2./0.800499 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta);
  return eff;
}

double eff2012IsoParkedTauMC_Arun_mvaTight(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.861041 * 0.5 * (TMath::Erf((pt - 41.5791)/2./0.768280 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= effDrop2012IsoParkedTau_Arun_mvaTight(pt, eta);
  return eff;
}

double eff2012IsoParkedTauMC_Arun_mvaVTight(double pt, double eta, bool applyEffDrop) 
{
  double eff = 0.877077 * 0.5 * (TMath::Erf((pt - 41.6020)/2./0.768319 /sqrt(pt)) + 1.);
  if ( applyEffDrop ) eff *= effDrop2012IsoParkedTau_Arun_mvaVTight(pt, eta);
  return eff;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
// Integral of Crystal Ball function for fitting trigger efficiency turn-on curves
// (code from Pascal Paganini)
//

double integralCrystalBall(double m, double m0, double sigma, double alpha, double n, double norm) 
{
  const double sqrtPiOver2 = 1.2533141373;
  const double sqrt2 = 1.4142135624;
  
  double sig = fabs((double)sigma);
  
  double t = (m - m0)/sig;
  
  if (alpha < 0) t = -t;
  
  double absAlpha = fabs(alpha / sig);
  double a = TMath::Power(n/absAlpha, n)*exp(-0.5*absAlpha*absAlpha);
  double b = absAlpha - n/absAlpha;
  
  if ( a >= std::numeric_limits<double>::max() ) return -1.;
  
  double ApproxErf;
  double arg = absAlpha / sqrt2;
  if      ( arg >  5. ) ApproxErf =  1;
  else if ( arg < -5. ) ApproxErf = -1;
  else                  ApproxErf = erf(arg);
  
  double leftArea = (1 + ApproxErf) * sqrtPiOver2;
  double rightArea = ( a * 1/TMath::Power(absAlpha - b,n-1)) / (n - 1);
  double area = leftArea + rightArea;
  
  if ( t <= absAlpha ) {
    arg = t / sqrt2;
    if      ( arg >  5.) ApproxErf =  1;
    else if ( arg < -5.) ApproxErf = -1;
    else                 ApproxErf = erf(arg);
    return norm * (1 + ApproxErf) * sqrtPiOver2 / area;
  } else {
    return norm * (leftArea +  a * (1/TMath::Power(t-b, n - 1) - 1/TMath::Power(absAlpha - b, n - 1)) / (1 - n)) / area;
  }
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// CV: PFJet320 trigger turn-on curves determined by Arun and me

double effPFJet320_vs_jetPt(double pt, double eta) 
{
  double absEta = TMath::Abs(eta);
  double m = pt;
  if ( m < 200. ) m = 200.; // CV: restrict m to Pt-range actually fitted
  if ( m > 500. ) m = 500.;
  double m0, sigma, alpha, n, norm;
  if ( absEta < 1.7 ) {
    m0    = 3.32065e+02;
    sigma = 2.27061e+01;
    alpha = 7.58443e+02;
    n     = 4.67695e+00;
    norm  = 9.96083e-01;
  } else if ( absEta < 1.9 ) {
    m0    = 3.36801e+02;
    sigma = 1.99752e+01;
    alpha = 5.00000e+02;
    n     = 2.00000e+00;
    norm  = 9.97946e-01;
  } else {
    m0    = 3.40758e+02;
    sigma = 1.82267e+01;
    alpha = 2.14371e+03;
    n     = 3.10895e+00;
    norm  = 9.98466e-01;
  }
  return TMath::Max(0., integralCrystalBall(m, m0, sigma, alpha, n, norm)); 
}

double effPFJet320MC_vs_jetPt(double pt, double eta) 
{
  double absEta = TMath::Abs(eta);
  double m = pt;
  if ( m < 200. ) m = 200.; // CV: restrict m to Pt-range actually fitted
  if ( m > 500. ) m = 500.;
  double m0, sigma, alpha, n, norm;
  if ( absEta < 1.7 ) {
    m0    = 3.29159e+02;
    sigma = 1.76891e+01;
    alpha = 1.90518e+03;
    n     = 1.13426e+01;
    norm  = 9.97524e-01;
  } else if ( absEta < 1.9 ) {
    m0    = 3.30077e+02;
    sigma = 1.77766e+01;
    alpha = 5.00000e+02;
    n     = 2.00000e+00;
    norm  = 9.98708e-01;
  } else {
    m0    = 3.27193e+02;
    sigma = 1.61665e+01;
    alpha = 2.43766e+03;
    n     = 1.87689e+01;
    norm  = 9.98651e-01;
  }
  return TMath::Max(0., integralCrystalBall(m, m0, sigma, alpha, n, norm)); 
}

double effPFJet320MC_vs_tauPt_cutMedium(double pt, double eta) 
{
  double m = pt;
  if ( m < 200. ) m = 200.;
  if ( m > 500. ) m = 500.;
  double m0    = 3.11593e+02;
  double sigma = 1.97325e+01;
  double alpha = 2.93861e+01;
  double n     = 1.27852e+02;
  double norm  = 9.83784e-01;
  return TMath::Max(0., integralCrystalBall(m, m0, sigma, alpha, n, norm)); 
}

double effPFJet320MC_vs_tauPt_mvaTight(double pt, double eta) 
{
  double m = pt;
  if ( m < 200. ) m = 200.;
  if ( m > 500. ) m = 500.;
  double m0    = 3.12211e+02;
  double sigma = 2.16338e+01;
  double alpha = 4.06313e+01;
  double n     = 1.08321e+01;
  double norm  = 9.84004e-01;
  return TMath::Max(0., integralCrystalBall(m, m0, sigma, alpha, n, norm)); 
}

double effPFJet320MC_vs_tauPt_mvaVTight(double pt, double eta) 
{
  double m = pt;
  if ( m < 200. ) m = 200.;
  if ( m > 500. ) m = 500.;
  double m0    = 3.12430e+02;
  double sigma = 2.17269e+01;
  double alpha = 4.23238e+01;
  double n     = 1.57691e+02;
  double norm  = 9.81625e-01;
  return TMath::Max(0., integralCrystalBall(m, m0, sigma, alpha, n, norm)); 
}

double effPFJet320(double tauPt, double jetPt, double eta) 
{
  double eff = effPFJet320MC_vs_tauPt_mvaTight(tauPt, eta);
  if ( jetPt > 200. ) { // CV: trigger efficiency for jets of Pt < 200 GeV is zero
    double effCorr_data = effPFJet320_vs_jetPt(jetPt, eta);
    double effCorr_mc = effPFJet320MC_vs_jetPt(jetPt, eta);
    if ( effCorr_mc > 0. ) {
      eff *= (effCorr_data/effCorr_mc);
      if ( eff > 1. ) eff = 1.;
    }
  }
  return eff;
}

double effPFJet320MC(double tauPt, double jetPt, double eta)
{
  return effPFJet320MC_vs_tauPt_mvaTight(tauPt, eta);
}
//-------------------------------------------------------------------------------
