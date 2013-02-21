#include <math.h> 
#include "TMath.h" 
#include <limits>
#include <iostream>

class ratioEfficiencyTest {

public:
  ratioEfficiencyTest(double m0, double sigma, double alpha, double n, double norm):
    m0_(m0), sigma_(sigma), alpha_(alpha), n_(n), norm_(norm)
  {} ;
  double efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const ;
  double turnOn(double pt) const ;

private:
  double m0_; 
  double sigma_; 
  double alpha_; 
  double n_; 
  double norm_;

} ;

double ratioEfficiencyTest::efficiency(double m, double m0, double sigma, double alpha, double n, double norm) const 
 { 
   //std::cout << "Efficiency for pt " << m << std::endl;

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


double ratioEfficiencyTest::turnOn(double pt) const
{
  double r = 0 ;
  double effdata = efficiency(pt,m0_,sigma_,alpha_,n_,norm_) ;

  r = effdata ;
  return r ;
}




