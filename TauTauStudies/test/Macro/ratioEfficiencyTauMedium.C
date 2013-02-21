 /***************************************************************************** 
  *                                                                           * 
  *****************************************************************************/ 
#include <math.h> 
#include "TMath.h" 
#include <limits>


class ratioEfficiencyTauMedium {
public:
  ratioEfficiencyTauMedium(){} ;
  double efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const ;
  double ratio(double pt) const ;
  double dataEfficiency(double pt) const;
} ;

double ratioEfficiencyTauMedium::efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const 
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


double ratioEfficiencyTauMedium::ratio(double pt) const
{
  double r = 0 ;
  if (pt<15) return r ;

  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;
  //2011A data 369.89 pb-1
  meandata=19.3783;
  sigmadata=1.93142;
  alphadata=4.43251;
  ndata=34.3245 ;
  normdata=0.869495;

  meanMC=18.3632 ; sigmaMC=1.05792  ; alphaMC=0.409811 ; nMC=4.28198 ; normMC=0.841393 ; //PU reweighting, Summer11_S4


  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;
  double effMC = efficiency(pt,meanMC,sigmaMC,alphaMC,nMC,normMC) ;

  if (effMC>0) r = effdata/effMC ;
  return r ;
}

double ratioEfficiencyTauMedium::dataEfficiency(double pt) const
{
  double r = 0 ;
  if (pt<15) return r ;

  double meandata,sigmadata,alphadata,ndata,normdata ;
  double meanMC,sigmaMC,alphaMC,nMC,normMC ;
  //2011A data 369.89 pb-1
  meandata=19.3783;
  sigmadata=1.93142;
  alphadata=4.43251;
  ndata=34.3245 ;
  normdata=0.869495;

  meanMC=18.3632 ; sigmaMC=1.05792  ; alphaMC=0.409811 ; nMC=4.28198 ; normMC=0.841393 ; //PU reweighting, Summer11_S4


  double effdata = efficiency(pt,meandata,sigmadata,alphadata,ndata,normdata) ;

  return  effdata;
}
