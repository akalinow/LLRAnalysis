import FWCore.ParameterSet.Config as cms


######################################################################################
simpleCutsWP95 = "(userFloat('nHits')<=1  && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.8 && userFloat('dEta')<0.007 && userFloat('HoE')<0.15) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.7 && userFloat('dEta')<0.01 && userFloat('HoE')<0.07) ))"

simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 &&  ( (pt>=20 && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.06 && userFloat('dEta')<0.004 && userFloat('HoE')<0.04) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.007 && userFloat('HoE')<0.025) )) || (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( (isEB && userFloat('sihih')<0.01 && userFloat('dPhi')<0.03 && userFloat('dEta')<0.004 && userFloat('HoE')<0.025) || (isEE && userFloat('sihih')<0.03 && userFloat('dPhi')<0.02 && userFloat('dEta')<0.005 && userFloat('HoE')<0.025) ) )  ) )"

tag = cms.EDFilter("PATElectronRefSelector",
                   src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                   cut = cms.string('pt>25.0 && abs(eta)<2.4 && !isEBEEGap && abs(userFloat("dxyWrtPV"))<0.02 && abs(userFloat("dzWrtPV"))<0.2 && userInt("antiConv")>0.5 && userFloat("PFRelIsoDB04v2")<0.10'+" && "+simpleCutsWP80),
                   filter = cms.bool(False)
                   )
######################################################################################
probe = cms.EDFilter("PATElectronRefSelector",
                     src = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
                     cut = cms.string('pt>15.0 && abs(eta)<2.4'),
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
                            pdgId = cms.vint32(11,-11),
                            src = cms.InputTag("tag"),
                            distMin = cms.double(0.15),
                            matched = cms.InputTag("genParticles")
                            )

probeMcMatch = cms.EDProducer("MCTruthDeltaRMatcherNew",
                            pdgId = cms.vint32(11,-11),
                            src = cms.InputTag("probe"),
                            distMin = cms.double(0.15),
                            matched = cms.InputTag("genParticles")
                            )
######################################################################################
elecTnP = cms.EDAnalyzer("TagProbeFitTreeProducer",
                         tagProbePairs = cms.InputTag("tnp"),
                         arbitration   = cms.string("OneProbe"),
                         variables = cms.PSet(
    pt = cms.string("pt"),
    iso= cms.string('userFloat("PFRelIsoDB04v2")'),
    abseta = cms.string("abs(eta)"),
    eta =  cms.string("eta"),
    phi = cms.string("phi"),
    ),
                         flags = cms.PSet(
    elecID95    =  cms.string(simpleCutsWP95),
    elecID80    =  cms.string(simpleCutsWP80),
    elecIso     =  cms.string('userFloat("PFRelIsoDB04v2")<0.1'),
    elecAgC     =  cms.string('userInt("antiConv")>0.5'),
    elecHLT1    =  cms.string('triggerObjectMatchesByFilter("hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter").size()'),
    elecHLT2    =  cms.string('triggerObjectMatchesByFilter("hltEle15CaloIdVTCaloIsoTTrkIdTTrkIsoTTrackIsoFilter").size()'),
    ),
                         tagVariables = cms.PSet(
    hlt1 = cms.string('triggerObjectMatchesByPath("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*",0,0).size()'),
    hlt2 = cms.string('triggerObjectMatchesByPath("HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v*",0,0).size()'),
    pt   = cms.string("pt"),                         
    eta  = cms.string("eta"),
    phi  = cms.string("phi"),
    pfRelIso   =  cms.string("userFloat('PFRelIsoDB04v2')"),
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
    objects = cms.InputTag("selectedPatElectronsTriggerMatchUserEmbedded"),
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
    elecTnP                          
    )
