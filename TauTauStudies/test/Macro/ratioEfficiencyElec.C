#include <math.h> 
#include "TMath.h" 
#include <limits>


class ratioEfficiencyElec {
public:
  ratioEfficiencyElec(){} ;
  double efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const ;
  double ratio(double pt, bool EB=true) const ;
  double dataEfficiency(double pt, bool EB=true) const ;
} ;

double ratioEfficiencyElec::efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const 
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


double ratioEfficiencyElec::ratio(double pt, bool EB) const
{
  double ratio_ = 0 ;
  if (pt<15) return ratio_ ;

  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;

  if (EB) {
    meandata=15.1605 ; sigmadata=0.62581 ; alphadata=0.896987 ; ndata=1.64618 ; normdata=0.978371 ;
    meanMC=14.8362 ; sigmaMC=0.62036 ; alphaMC=1.18952 ; nMC=1.33197 ; normMC=0.995733 ;
  } else {
    meandata=16.1936  ; sigmadata=1.2746 ; alphadata=1.56386  ; ndata=1.62749  ; normdata=1 ;
    meanMC=14.4009 ; sigmaMC=0.723322 ; alphaMC= 0.699329  ; nMC=1.74205  ; normMC=1 ;
  }

  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;
  double effMC = efficiency(pt,meanMC,sigmaMC,alphaMC,nMC,normMC) ;

  if (effMC>0) ratio_ = effdata/effMC ;
  return ratio_ ;
}

double ratioEfficiencyElec::dataEfficiency(double pt, bool EB) const
{
  double ratio_ = 0 ;
  if (pt<15) return ratio_ ;

  double meandata,sigmadata,alphadata,ndata,normdata ;

  if (EB) {
    meandata=15.1605 ; sigmadata=0.62581 ; alphadata=0.896987 ; ndata=1.64618 ; normdata=0.978371 ;
  } else {
    meandata=16.1936  ; sigmadata=1.2746 ; alphadata=1.56386  ; ndata=1.62749  ; normdata=1 ;
  }

  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;
  ratio_ = effdata ;
  return ratio_ ;
}



