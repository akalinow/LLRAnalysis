import FWCore.ParameterSet.Config as cms


######################################################################################
simpleCutsWP95 = "(userFloat('nHits')<=1 && userFloat('dist')>-999 && userFloat('dcot')>-999 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.15) ) && (pt>=20 || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) )) )"
simpleCutsWP80 = "(userFloat('nHits')==0 && userFloat('dist')>0.02 && userFloat('dcot')>0.02 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) )  && (pt>=20 || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) )) )"
simpleCutsWP70 = "(userFloat('nHits')==0 && userFloat('dist')>0.02 && userFloat('dcot')>0.02 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.004 && userFloat('HoE')<0.025) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.02 && userFloat('dEta')<0.005 && userFloat('HoE')<0.15) )  && (pt>=20 || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) )) )"

tag = cms.EDFilter("PATElectronRefSelector",
                   src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                   cut = cms.string('pt>25.0 && abs(eta)<2.4 && !isEBEEGap && abs(userFloat("dxyWrtPV"))<0.02 && abs(userFloat("dzWrtPV"))<0.2'),
                   filter = cms.bool(False)
                   )
tag70 = cms.EDFilter("PATElectronRefSelector",
                     src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                     cut = cms.string(tag.cut.value()+" && "+simpleCutsWP70),
                     filter = cms.bool(False)
                     )
######################################################################################
probe = cms.EDFilter("PATTauRefSelector",
                     src = cms.InputTag("selectedPatTaus"),
                     cut = cms.string('pt>20.0 && abs(eta)<2.3'),
                     filter = cms.bool(False)
                     )
probeID = cms.EDFilter("PATTauRefSelector",
                     src = cms.InputTag("selectedPatTaus"),
                     cut = cms.string(probe.cut.value()+" && tauID('decayModeFinding')>0.5"),
                     filter = cms.bool(False)
                     )
probeIDLoose = cms.EDFilter("PATTauRefSelector",
                            src = cms.InputTag("selectedPatTaus"),
                            cut = cms.string(probeID.cut.value()+" && tauID('byLooseIsolation')>0.5"),
                            filter = cms.bool(False)
                            )
probeIDMedium = cms.EDFilter("PATTauRefSelector",
                             src = cms.InputTag("selectedPatTaus"),
                             cut = cms.string(probeID.cut.value()+" && tauID('byMediumIsolation')>0.5"),
                             filter = cms.bool(False)
                             )
probeIDTight = cms.EDFilter("PATTauRefSelector",
                            src = cms.InputTag("selectedPatTaus"),
                            cut = cms.string(probeID.cut.value()+" && tauID('byTightIsolation')>0.5"),
                            filter = cms.bool(False)
                            )
######################################################################################
tnp70IDLoose = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tag70 probeIDLoose"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )
tnp70IDMedium = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tag70 probeIDMedium"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )
tnp70IDTight = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tag70 probeIDTight"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )

######################################################################################
oneTp70IDLoose = cms.EDFilter("CandViewCountFilter",
                              src = cms.InputTag("tnp70IDLoose"),
                              minNumber = cms.uint32(1)
                              )
oneTp70IDMedium = cms.EDFilter("CandViewCountFilter",
                               src = cms.InputTag("tnp70IDMedium"),
                               minNumber = cms.uint32(1)
                               )
oneTp70IDTight = cms.EDFilter("CandViewCountFilter",
                            src = cms.InputTag("tnp70IDTight"),
                            minNumber = cms.uint32(1)
                            )
######################################################################################

tag70McMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                              pdgId = cms.vint32(11,-11),
                              src = cms.InputTag("tag70"),
                              distMin = cms.double(0.15),
                              matched = cms.InputTag("genParticles")
                              )

probeIDLooseMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(11,-11),
                                     src = cms.InputTag("probeIDLoose"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
probeIDMediumMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(11,-11),
                                     src = cms.InputTag("probeIDMedium"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
probeIDTightMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(11,-11),
                                     src = cms.InputTag("probeIDTight"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
######################################################################################

etoTau70IDLoose = cms.EDAnalyzer("TagProbeFitTreeProducer",
                                 tagProbePairs = cms.InputTag("tnp70IDLoose"),
                                 arbitration   = cms.string("OneProbe"),
                                 #massForArbitration = cms.double(91),
                                 variables = cms.PSet(
    pt = cms.string("pt"), 
    abseta = cms.string("abs(eta)"),
    eta =  cms.string("eta"),
    phi = cms.string("phi"),
    emFraction = cms.string("emFraction"),
    hcalTotOverPLead = cms.string("hcalTotOverPLead"),
    #hcalEnergy =  cms.string("leadPFChargedHadrCand.hcalEnergy"),
    #ecalEnergy =  cms.string("leadPFChargedHadrCand.ecalEnergy"),
    #hcalEnergyLeadPFCand =  cms.string("leadPFCand.hcalEnergy"),
    #ecalEnergyLeadPFCand =  cms.string("leadPFCand.ecalEnergy"),
    #bremsRecoveryEOverPLead = cms.string("bremsRecoveryEOverPLead"),
    electronPreIDOutput = cms.string("electronPreIDOutput"),
    #electronPreIDOutputLeadPFCand = cms.string("leadPFCand.mva_e_pi"),
    #ecalStripSumEOverPLead = cms.string("ecalStripSumEOverPLead"),
    leadPFChargedHadrCandPt = cms.string("leadPFChargedHadrCand.pt"),
    #leadPFChargedHadrCandTrackP = cms.string("leadPFChargedHadrCand.trackRef.p"),
    #leadPFCandPt = cms.string("leadPFCand.pt"),
    #leadPFCandP = cms.string("leadPFCand.p"),
    signalPFChargedHadrCands = cms.string("signalPFChargedHadrCands.size"),
    signalPFGammaCands = cms.string("signalPFGammaCands.size"),
    #nHitsLeadTrack = cms.string("leadPFChargedHadrCand.trackRef.numberOfValidHits"),
    tauVisMass = cms.string("mass"),
    ),
                                 flags = cms.PSet(
    tauAntiELoose    =  cms.string('tauID("againstElectronLoose")>0.5'),
    tauAntiEMedium   =  cms.string('tauID("againstElectronMedium")>0.5'),
    tauAntiETight    =  cms.string('tauID("againstElectronTight")>0.5'),
    tauAntiECrackRem =  cms.string('tauID("againstElectronCrackRem")>0.5'),
    tauAntiMuLoose   =  cms.string('tauID("againstMuonLoose")>0.5'),
    tauAntiMuTight   =  cms.string('tauID("againstMuonTight")>0.5'),
    ),
                                 tagVariables = cms.PSet(
    hlt1 = cms.string('triggerObjectMatchesByPath("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*").size()'),
    hlt2 = cms.string('triggerObjectMatchesByPath("HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*").size()'),
    pt = cms.string("pt"), 
    eta = cms.string("eta"),
    phi = cms.string("phi"),
    nHits = cms.string("userFloat('nHits')"),
    dist = cms.string("userFloat('dist')"),
    dcot = cms.string("userFloat('dcot')"),
    sihih = cms.string("userFloat('sihih')"),
    dPhi= cms.string("userFloat('dPhi')"),
    dEta= cms.string("userFloat('dEta')"),
    HoE= cms.string("userFloat('HoE')"),
    pfRelIso =  cms.string("userFloat('PFRelIso04')"),
    Mt =  cms.InputTag("addUserVariables","Mt"),
    puMCWeight =  cms.InputTag("addUserVariables","puMCWeight")
    ),
                                 tagFlags = cms.PSet(),
                                 pairVariables = cms.PSet(
    tnpCharge = cms.string('charge'),
    deltaR = cms.string('sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  )  )')
    ),
                                 pairFlags = cms.PSet(),
                                 isMC = cms.bool( True ),
                                 tagMatches = cms.InputTag("tag70McMatch") ,
                                 probeMatches  = cms.InputTag("probeIDLooseMcMatch"),
                                 motherPdgId = cms.vint32(23),
                                 makeMCUnbiasTree = cms.bool(True),
                                 checkMotherInUnbiasEff = cms.bool(True),
                                 allProbes = cms.InputTag("probeIDLoose"),
                                 addRunLumiInfo = cms.bool(True),
                                 addEventVariablesInfo = cms.bool(True),
                                 )

etoTau70IDMedium = etoTau70IDLoose.clone(
    tagProbePairs = cms.InputTag("tnp70IDMedium"),
    probeMatches = cms.InputTag("probeIDMediumMcMatch"),
    allProbes = cms.InputTag("probeIDMedium"),
    )
etoTau70IDTight = etoTau70IDLoose.clone(
    tagProbePairs = cms.InputTag("tnp70IDTight"),
    probeMatches = cms.InputTag("probeIDTightMcMatch"),
    allProbes = cms.InputTag("probeIDTight"),
    )
#############################################################################

tag70METPair = cms.EDProducer(
    "CandViewShallowCloneCombiner",
    decay = cms.string("tag70 patMETsPFlow"),
    cut   = cms.string('sqrt((daughter(0).pt+daughter(1).pt)*(daughter(0).pt+daughter(1).pt)-pt*pt)>50'),
    checkCharge = cms.bool(False)
    )

tag70METPairFilter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tag70METPair"),
    minNumber = cms.uint32(1),
    )

addUserVariables = cms.EDProducer(
    "UserDefinedVariables",
    objects = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
    met = cms.InputTag("patMETsPFlow"),
    isMC = cms.bool(True)
    )

#############################################################################

sequence70 = cms.Sequence(
    (tag70+probeIDLoose+probeIDMedium+probeIDTight)*
    (tnp70IDLoose+tnp70IDMedium+tnp70IDTight)*
    oneTp70IDLoose+
    #(tag70METPair*(~tag70METPairFilter))+
    addUserVariables*
    (tag70McMatch+probeIDLooseMcMatch+probeIDMediumMcMatch+probeIDTightMcMatch)*
    (etoTau70IDLoose+etoTau70IDMedium+etoTau70IDTight)                           
    )
