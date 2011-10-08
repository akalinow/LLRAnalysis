

{

  gSystem->Load("computeElecTnPMacroMC_C.so");

  //plot("elecTnP","elecID95","pt>15 && pt<20","15to20",1.0,0.5,60,120, 24, false);
  //plot("elecTnP","elecID95","pt>20 && pt<25","20to25",1.0,0.5,60,120, 24, false);
  //plot("elecTnP","elecID95","pt>25 && pt<30","25to30",1.0,0.5,60,120, 24, false);
  //plot("elecTnP","elecID95","pt>30 && pt<50","30to50",1.0,0.5,60,120, 24, false);

  plot("elecTnP","elecID80","pt>15 && pt<20","15to20",1.0,0.5,60,120, 24, false);

  return;

  plot("elecTnP","elecID80","pt>20 && pt<25","20to25",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecID80","pt>25 && pt<30","25to30",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecID80","pt>30 && pt<50","30to50",1.0,0.5,60,120, 24, true);



  plot("elecTnP","elecIso","pt>15 && pt<20 && elecID80","15to20ID80",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecIso","pt>20 && pt<25 && elecID80","20to25ID80",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecIso","pt>25 && pt<30 && elecID80","25to30ID80",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecIso","pt>30 && pt<50 && elecID80","30to50ID80",1.0,0.5,60,120, 24, true);

  plot("elecTnP","elecHLT","pt>15 && pt<20 && elecID80 && elecIso","15to20ID80Iso",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecHLT","pt>20 && pt<25 && elecID80 && elecIso","20to25ID80Iso",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecHLT","pt>25 && pt<30 && elecID80 && elecIso","25to30ID80Iso",1.0,0.5,60,120, 24, true);
  plot("elecTnP","elecHLT","pt>30 && pt<50 && elecID80 && elecIso","30to50ID80Iso",1.0,0.5,60,120, 24, true);


}
