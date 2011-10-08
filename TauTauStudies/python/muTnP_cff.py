import FWCore.ParameterSet.Config as cms


######################################################################################

muIDCut = "isTrackerMuon && isGlobalMuon && numberOfMatches>=2 && globalTrack.isNonnull && globalTrack.hitPattern.numberOfValidMuonHits>=1 && globalTrack.hitPattern.numberOfValidPixelHits>=1 && globalTrack.hitPattern.numberOfValidTrackerHits>10 && globalTrack.normalizedChi2<10 && globalTrack.ptError/globalTrack.pt<0.1"

tag = cms.EDFilter("PATMuonRefSelector",
                   src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
                   cut = cms.string('pt>25.0 && abs(eta)<2.1 && userFloat("PFRelIsoDB04v2")<0.1 && abs(userFloat("dxyWrtPV"))<0.045 && abs(userFloat("dzWrtPV"))<0.2 && '+muIDCut),
                   filter = cms.bool(False)
                   )
######################################################################################
probe = cms.EDFilter("PATMuonRefSelector",
                     src = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
                     cut = cms.string('pt>15.0 && abs(eta)<2.1'),
                     filter = cms.bool(False)
                     )
######################################################################################
tnp = cms.EDProducer("CandViewShallowCloneCombiner",
                     decay = cms.string("tag probe"),
                     cut   = cms.string("60 < mass < 120"),
                     checkCharge = cms.bool(False),   
                     )
######################################################################################
oneTp = cms.EDFilter("CandViewCountFilter",
                     src = cms.InputTag("tnp"),
                     minNumber = cms.uint32(1)
                     )
######################################################################################
tagMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                            pdgId = cms.vint32(13,-13),
                            src = cms.InputTag("tag"),
                            distMin = cms.double(0.15),
                            matched = cms.InputTag("genParticles")
                            )

probeMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                              pdgId = cms.vint32(13,-13),
                              src = cms.InputTag("probe"),
                              distMin = cms.double(0.15),
                              matched = cms.InputTag("genParticles")
                              )
######################################################################################
muTnP = cms.EDAnalyzer("TagProbeFitTreeProducer",
                       tagProbePairs = cms.InputTag("tnp"),
                       arbitration   = cms.string("OneProbe"),
                       #massForArbitration = cms.double(91),
                       variables = cms.PSet(
    pt = cms.string("pt"),
    iso= cms.string('userFloat("PFRelIsoDB04v2")'),
    abseta = cms.string("abs(eta)"),
    eta =  cms.string("eta"),
    phi = cms.string("phi"),
    ),
                         flags = cms.PSet(
    muID      =  cms.string(muIDCut),
    muIDRel   =  cms.string('isGlobalMuon'),
    muIso     =  cms.string('userFloat("PFRelIsoDB04v2")<0.1'),
    muHLT12   =  cms.string('triggerObjectMatchesByFilter("hltSingleMuIsoL3IsoFiltered12").size()'),
    muHLT15   =  cms.string('triggerObjectMatchesByFilter("hltSingleMuIsoL3IsoFiltered15").size()'),
    ),
                         tagVariables = cms.PSet(
    hlt = cms.string('triggerObjectMatchesByPath("HLT_IsoMu24_v*",0,0).size()'),
    pt = cms.string("pt"), 
    eta = cms.string("eta"),
    phi = cms.string("phi"),
    pfRelIso =  cms.string("userFloat('PFRelIsoDB04v2')"),
    puMCWeight =  cms.InputTag("addUserVariables","puMCWeight"),
    Mt         =  cms.InputTag("addUserVariables","Mt"),
    genDecay   =  cms.InputTag("addUserVariables","genDecay"),
    numGenPU   =  cms.InputTag("addUserVariables","numGenPU"),
    ),
                         tagFlags = cms.PSet(),
                         pairVariables = cms.PSet(
    tnpCharge = cms.string('charge'),
    deltaR = cms.string('sqrt((daughter(0).eta-daughter(1).eta)*(daughter(0).eta-daughter(1).eta)+  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  ) *  min( abs(daughter(0).phi-daughter(1).phi), 2*3.141 - abs(daughter(0).phi-daughter(1).phi)  )  )')
    ),
                       pairFlags = cms.PSet(),
                       isMC = cms.bool( True ),
                       tagMatches = cms.InputTag("tagMcMatch") ,
                       probeMatches  = cms.InputTag("probeMcMatch"),
                       motherPdgId = cms.vint32(23),
                       makeMCUnbiasTree = cms.bool(True),
                       checkMotherInUnbiasEff = cms.bool(True),
                       allProbes = cms.InputTag("probe"),
                       addRunLumiInfo = cms.bool(True),
                       addEventVariablesInfo = cms.bool(True),
                       )

#############################################################################

addUserVariables = cms.EDProducer(
    "UserDefinedVariables",
    objects = cms.InputTag("selectedPatMuonsTriggerMatchUserEmbedded"),
    met = cms.InputTag("patMETsPFlow"),
    isMC = cms.bool(True)
    )

#############################################################################

sequence = cms.Sequence(
    (tag+probe)*
    tnp*
    oneTp+
    addUserVariables*
    (tagMcMatch+probeMcMatch)*
    muTnP                          
    )
