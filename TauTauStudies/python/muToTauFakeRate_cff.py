import FWCore.ParameterSet.Config as cms


######################################################################################
tag = cms.EDFilter("PATMuonRefSelector",
                   src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
                   cut = cms.string('pt>20.0 && abs(eta)<2.1 && abs(userFloat("dxyWrtPV"))<0.02 && abs(userFloat("dzWrtPV"))<0.2'),
                   filter = cms.bool(False)
                   )
tagVBTF = cms.EDFilter("PATMuonRefSelector",
                       src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
                       cut = cms.string(tag.cut.value()+" && isTrackerMuon && numberOfMatches>=2 && globalTrack.isNonnull && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.hitPattern.numberOfValidTrackerHits>10 && globalTrack.normalizedChi2<10 && globalTrack.ptError/globalTrack.pt<0.1"),
                       filter = cms.bool(False)
                       )
######################################################################################
probe = cms.EDFilter("PATTauRefSelector",
                     src = cms.InputTag("selectedPatTaus"),
                     cut = cms.string('pt>15.0 && abs(eta)<2.5'),
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
tnpVBTFIDLoose = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tagVBTF probeIDLoose"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )
tnpVBTFIDMedium = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tagVBTF probeIDMedium"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )
tnpVBTFIDTight = cms.EDProducer("CandViewShallowCloneCombiner",
                            decay = cms.string("tagVBTF probeIDTight"),
                            cut   = cms.string("40 < mass < 120"),
                            checkCharge = cms.bool(False),   
                            )

######################################################################################
oneTpVBTFIDLoose = cms.EDFilter("CandViewCountFilter",
                              src = cms.InputTag("tnpVBTFIDLoose"),
                              minNumber = cms.uint32(1)
                              )
oneTpVBTFIDMedium = cms.EDFilter("CandViewCountFilter",
                            src = cms.InputTag("tnpVBTFIDMedium"),
                            minNumber = cms.uint32(1)
                            )
oneTpVBTFIDTight = cms.EDFilter("CandViewCountFilter",
                            src = cms.InputTag("tnpVBTFIDTight"),
                            minNumber = cms.uint32(1)
                            )
######################################################################################

tagVBTFMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                pdgId = cms.vint32(13,-13),
                                src = cms.InputTag("tagVBTF"),
                                distMin = cms.double(0.15),
                                matched = cms.InputTag("genParticles")
                                )

probeIDLooseMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(13,-13),
                                     src = cms.InputTag("probeIDLoose"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
probeIDMediumMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(13,-13),
                                     src = cms.InputTag("probeIDMedium"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
probeIDTightMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                                     pdgId = cms.vint32(13,-13),
                                     src = cms.InputTag("probeIDTight"),
                                     distMin = cms.double(0.15),
                                     matched = cms.InputTag("genParticles")
                                     )
######################################################################################

etoTauVBTFIDLoose = cms.EDAnalyzer("TagProbeFitTreeProducer",
                                 tagProbePairs = cms.InputTag("tnpVBTFIDLoose"),
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
    #leadPFChargedHadrCandTrackPt = cms.string("leadPFChargedHadrCand.trackRef.pt"),
    #leadPFChargedHadrCandTrackP = cms.string("leadPFChargedHadrCand.trackRef.p"),
    #leadPFCandPt = cms.string("leadPFCand.pt"),
    #leadPFCandP = cms.string("leadPFCand.p"),
    leadPFChargedHadrCandPt = cms.string("leadPFChargedHadrCand.pt"),
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
    hlt1 = cms.string('triggerObjectMatchesByPath("HLT_IsoMu17_v*").size()'),
    hlt2 = cms.string('triggerObjectMatchesByPath("HLT_IsoMu24_v*").size()'),
    pt = cms.string("pt"), 
    eta = cms.string("eta"),
    phi = cms.string("phi"),
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
                                 tagMatches = cms.InputTag("tagVBTFMcMatch") ,
                                 probeMatches  = cms.InputTag("probeIDLooseMcMatch"),
                                 motherPdgId = cms.vint32(23),
                                 makeMCUnbiasTree = cms.bool(True),
                                 checkMotherInUnbiasEff = cms.bool(True),
                                 allProbes = cms.InputTag("probeIDLoose"),
                                 addRunLumiInfo = cms.bool(True),
                                 addEventVariablesInfo = cms.bool(True),
                                 )

etoTauVBTFIDMedium = etoTauVBTFIDLoose.clone(
    tagProbePairs = cms.InputTag("tnpVBTFIDMedium"),
    probeMatches = cms.InputTag("probeIDMediumMcMatch"),
    allProbes = cms.InputTag("probeIDMedium"),
    )
etoTauVBTFIDTight = etoTauVBTFIDLoose.clone(
    tagProbePairs = cms.InputTag("tnpVBTFIDTight"),
    probeMatches = cms.InputTag("probeIDTightMcMatch"),
    allProbes = cms.InputTag("probeIDTight"),
    )
#############################################################################

tagVBTFMETPair = cms.EDProducer("CandViewShallowCloneCombiner",
                              decay = cms.string("tagVBTF patMETsPFlow"),
                              cut   = cms.string('sqrt((daughter(0).pt+daughter(1).pt)*(daughter(0).pt+daughter(1).pt)-pt*pt)>50'),
                              checkCharge = cms.bool(False)
                              )

tagVBTFMETPairFilter = cms.EDFilter("CandViewCountFilter",
                                  src = cms.InputTag("tagVBTFMETPair"),
                                  minNumber = cms.uint32(1),
                                  )
addUserVariables = cms.EDProducer(
    "UserDefinedVariables",
    objects = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    met = cms.InputTag("patMETsPFlow"),
    isMC = cms.bool(True)
    )

#############################################################################

sequenceVBTF = cms.Sequence(
    (tagVBTF+probeIDLoose+probeIDMedium+probeIDTight)*
    (tnpVBTFIDLoose+tnpVBTFIDMedium+tnpVBTFIDTight)*
    oneTpVBTFIDLoose+
    #(tagVBTFMETPair*(~tagVBTFMETPairFilter))+
    addUserVariables*
    (tagVBTFMcMatch+probeIDLooseMcMatch+probeIDMediumMcMatch+probeIDTightMcMatch)*
    (etoTauVBTFIDLoose+etoTauVBTFIDMedium+etoTauVBTFIDTight)                           
    )
