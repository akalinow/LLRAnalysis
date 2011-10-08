/***************************************************************************** 
  *                                                                           * 
  *****************************************************************************/ 
#include <math.h> 
#include "TMath.h" 
#include <limits>


class ratioEfficiencyTau10 {
public:
  ratioEfficiencyTau10(){} ;
  double efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const ;
  double ratio(double pt) const ;
  double dataEfficiency(double pt) const ;
  double mcEfficiency(double pt) const ;

} ;

double ratioEfficiencyTau10::efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const 
 { 
   const double sqrtPiOver2 = 1.2533141373;
   const double sqrt2 = 1.4142135624;

   double sig = fabs((double) sigma);
   
   double t = (m - m0)/sig ;
   
   if (alpha < 0)
     t = -t;

   double absAlpha = fabs(alpha / sig);
   double a = TMath::Power(n/absAlpha,n)*exp(-0.5*absAlpha*absAlpha);
   double b = absAlpha - n/absAlpha;

   if (a>=std::numeric_limits<double>::max()) return -1. ;

   double ApproxErf ;
   double arg = absAlpha / sqrt2 ;
   if (arg > 5.) ApproxErf = 1 ;
   else if (arg < -5.) ApproxErf = -1 ;
   else ApproxErf = erf(arg) ;

   double leftArea = (1 + ApproxErf) * sqrtPiOver2 ;
   double rightArea = ( a * 1/TMath::Power(absAlpha - b,n-1)) / (n - 1);
   double area = leftArea + rightArea;


   if ( t <= absAlpha ){
     arg = t / sqrt2 ;
     if (arg > 5.) ApproxErf = 1 ;
     else if (arg < -5.) ApproxErf = -1 ;
     else ApproxErf = erf(arg) ;
     return norm * (1 + ApproxErf) * sqrtPiOver2 / area ;
   }
   else{
     return norm * (leftArea +  a * (1/TMath::Power(t-b,n-1) - 1/TMath::Power(absAlpha - b,n-1)) / (1 - n)) / area ;
   }
  
 } 


double ratioEfficiencyTau10::ratio(double pt) const
{
  double r = 0 ;
  if (pt<15) return r ;

  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;

    meandata=5.0403 ; sigmadata=4.84108 ; alphadata=2.89048 ; ndata=26.6496 ; normdata=0.910342 ; //2011 data 0.832 fb-1
    meanMC=12.3718 ; sigmaMC=0.542694   ; alphaMC=0.208853 ; nMC=34.996 ; normMC=0.810989 ; //PU reweighting, Summer11_S4


  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;
  double effMC   = efficiency(pt,meanMC,sigmaMC,alphaMC,nMC,normMC) ;

  if (effMC>0) r = effdata/effMC ;
  return r ;
}

double ratioEfficiencyTau10::dataEfficiency(double pt) const
{
  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;
 
  meandata=5.0403 ; sigmadata=4.84108 ; alphadata=2.89048 ; ndata=26.6496 ; normdata=0.910342 ; //2011 data 0.832 fb-1
  meanMC=12.3718 ; sigmaMC=0.542694   ; alphaMC=0.208853 ; nMC=34.996 ; normMC=0.810989 ; //PU reweighting, Summer11_S4

  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;
  return effdata ;
}

double ratioEfficiencyTau10::mcEfficiency(double pt) const
{
  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;
 
  meandata=5.0403 ; sigmadata=4.84108 ; alphadata=2.89048 ; ndata=26.6496 ; normdata=0.910342 ; //2011 data 0.832 fb-1
  meanMC=12.3718 ; sigmaMC=0.542694   ; alphaMC=0.208853 ; nMC=34.996 ; normMC=0.810989 ; //PU reweighting, Summer11_S4

  double effMC   = efficiency(pt,meanMC,sigmaMC,alphaMC,nMC,normMC) ;
  return effMC ;
}
