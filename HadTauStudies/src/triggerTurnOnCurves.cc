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
// CV: turn-on curves determined by Arun

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

double eff2012IsoParkedTau_Arun_cutMedium(double pt, double eta) 
{
  return ( 0.919909 * 0.5 * (TMath::Erf((pt - 43.1755)/2./0.762391 /sqrt(pt)) + 1.) * (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta)) / 18.3); 
}

double effDrop2012IsoParkedTau_Arun_mvaTight(double pt, double eta) // CV: drop of tau trigger efficiency due to bug in PFlow config discovered in Spring 2012 
                                                                    //    (affects Summer'12 MC and 2012 run ABC data, L = 11 fb^-1)
{
  if ( pt < 140. ) return 1.;
  else return ( (1. - 1.10448e-03*(pt - 140.) + 4.09426e-07*square(pt - 140.)) );
}

double eff2012IsoParkedTau_Arun_mvaTight(double pt, double eta) 
{
  return ( 0.876216 * 0.5 * (TMath::Erf((pt - 42.7581)/2./0.752733 /sqrt(pt)) + 1.) * (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_mvaTight(pt, eta)) / 18.3);
}

double effDrop2012IsoParkedTau_Arun_mvaVTight(double pt, double eta) // CV: drop of tau trigger efficiency due to bug in PFlow config discovered in Spring 2012 
                                                                     //    (affects Summer'12 MC and 2012 run ABC data, L = 11 fb^-1)
{
  if ( pt < 140. ) return 1.;
  else return ( (1. - 1.09226e-03*(pt - 140.) + 4.00873e-07*square(pt - 140.)) );
}

double eff2012IsoParkedTau_Arun_mvaVTight(double pt, double eta) 
{
  return ( 0.894735 * 0.5 * (TMath::Erf((pt - 42.9676)/2./0.759977 /sqrt(pt)) + 1.) * (7.3 + 11.0 * effDrop2012IsoParkedTau_Arun_mvaTight(pt, eta)) / 18.3); 
}


double eff2012IsoParkedTauMC_Arun_cutMedium(double pt, double eta) 
{
  return ( 0.880128 * 0.5 * (TMath::Erf((pt - 41.8055)/2./0.800500 /sqrt(pt)) + 1.) * effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta));
}

double eff2012IsoParkedTauMC_Arun_mvaTight(double pt, double eta) 
{
  return ( 0.861033 * 0.5 * (TMath::Erf((pt - 41.5781)/2./0.768214 /sqrt(pt)) + 1.) * effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta)); 
}

double eff2012IsoParkedTauMC_Arun_mvaVTight(double pt, double eta) 
{
  return ( 0.877075 * 0.5 * (TMath::Erf((pt - 41.6020)/2./0.768297 /sqrt(pt)) + 1.) * effDrop2012IsoParkedTau_Arun_cutMedium(pt, eta)); 
}
//-------------------------------------------------------------------------------
