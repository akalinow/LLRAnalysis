import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.pfTools import *
from CommonTools.ParticleFlow.Isolation.tools_cfi import *

###################a#################################################
def addSelectedPFlowParticle(process,verbose=False):
    if verbose:
        print "[Info] Adding pf-particles (for pf-isolation and pf-seed pat-leptons)"
    process.load("CommonTools.ParticleFlow.ParticleSelectors.pfSortByType_cff")
    process.load("CommonTools.ParticleFlow.pfNoPileUp_cff")

    process.pfAllChargedCandidates = process.pfAllChargedHadrons.clone()
    process.pfAllChargedCandidates.pdgId.extend(process.pfAllMuons.pdgId.value())
    process.pfAllChargedCandidates.pdgId.extend(process.pfAllElectrons.pdgId.value())
    
    process.pfPU = cms.EDProducer(
        "TPPFCandidatesOnPFCandidates",
        enable =  cms.bool( True ),
        verbose = cms.untracked.bool( False ),
        name = cms.untracked.string("puPFCandidates"),
        topCollection = cms.InputTag("pfNoPileUp"),
        bottomCollection = cms.InputTag("particleFlow"),
        )
    process.pfAllChargedHadronsPU = process.pfAllChargedHadrons.clone(src='pfPU')

    #################################################################
    process.pfNoCharged = process.pfPU.clone(
        name = cms.untracked.string("noChargedPFCandidates"),
        topCollection = cms.InputTag("pfAllChargedHadrons"),
        bottomCollection = cms.InputTag("particleFlow"),
        )
    process.pfAllNeutral = cms.EDFilter(
        "PdgIdPFCandidateSelector",
        pdgId = cms.vint32(111, 130, 310, 2112, 22),
        src = cms.InputTag("pfNoCharged")
        )
    #################################################################

    process.pfCandidateSelectionByType = cms.Sequence(
        process.pfNoPileUpSequence *
        ( process.pfAllNeutralHadrons +
          process.pfAllChargedHadrons +
          process.pfAllChargedCandidates +
          process.pfAllPhotons +
          (process.pfPU * process.pfAllChargedHadronsPU )
          )  +
        process.pfAllMuons +
        process.pfAllElectrons +
        ( process.pfNoCharged+process.pfAllNeutral)
        )
    process.pfPileUp.Enable              = True
    process.pfPileUp.checkClosestZVertex = False
    process.pfPileUp.Vertices            = "offlinePrimaryVertices"
    process.pfAllMuons.src               = "particleFlow"
    process.pfAllElectrons.src           = "particleFlow"
    
    
###################a#################################################
def addPFMuonIsolation(process,module,postfix="",verbose=False):
    if verbose:
        print "[Info] Adding particle isolation to muon with postfix '"+postfix+"'"

    #if not hasattr(process, "pfCandidateSelectionByType"):
    #    addSelectedPFlowParticle(process,verbose=verbose)
        
    #setup correct src of isolated object
    setattr(process,"isoDepMuonWithCharged"+postfix,
            isoDepositReplace(module.muonSource,
                              'pfAllChargedCandidates'))
    setattr(process,"isoDepMuonWithChargedPU"+postfix,
            isoDepositReplace(module.muonSource,
                              'pfAllChargedHadronsPU'))
    setattr(process,"isoDepMuonWithNeutral"+postfix,
            isoDepositReplace(module.muonSource,
                              'pfAllNeutralHadrons'))
    setattr(process,"isoDepMuonWithPhotons"+postfix,
            isoDepositReplace(module.muonSource,
                              'pfAllPhotons'))

    #compute isolation values form deposits
    process.load("CommonTools.ParticleFlow.Isolation.pfMuonIsolationFromDeposits_cff")
    setattr(process,"isoValMuonWithChargedPU",
            process.isoValMuonWithCharged.clone())
    getattr(process,"isoValMuonWithChargedPU").deposits[0].src="isoDepMuonWithChargedPU"

    if postfix!="":
        setattr(process,"isoValMuonWithCharged"+postfix,
                process.isoValMuonWithCharged.clone())
        getattr(process,"isoValMuonWithCharged"+postfix).deposits[0].src="isoDepMuonWithCharged"+postfix
        setattr(process,"isoValMuonWithChargedPU"+postfix,
                process.isoValMuonWithChargedPU.clone())
        getattr(process,"isoValMuonWithChargedPU"+postfix).deposits[0].src="isoDepMuonWithChargedPU"+postfix
        setattr(process,"isoValMuonWithNeutral"+postfix,
                process.isoValMuonWithNeutral.clone())
        getattr(process,"isoValMuonWithNeutral"+postfix).deposits[0].src="isoDepMuonWithNeutral"+postfix
        setattr(process,"isoValMuonWithPhotons"+postfix,
                process.isoValMuonWithPhotons.clone())
        getattr(process,"isoValMuonWithPhotons"+postfix).deposits[0].src="isoDepMuonWithPhotons"+postfix
        
    setattr(process,"patMuonIsolationFromDepositsSequence"+postfix,
            cms.Sequence(getattr(process,"isoValMuonWithCharged"+postfix) +
                         getattr(process,"isoValMuonWithChargedPU"+postfix) +
                         getattr(process,"isoValMuonWithNeutral"+postfix) +
                         getattr(process,"isoValMuonWithPhotons"+postfix)
                         )
            )

    setattr(process,"patMuonIsoDepositsSequence"+postfix,
            cms.Sequence(getattr(process,"isoDepMuonWithCharged"+postfix) +
                         getattr(process,"isoDepMuonWithChargedPU"+postfix) +
                         getattr(process,"isoDepMuonWithNeutral"+postfix) +
                         getattr(process,"isoDepMuonWithPhotons"+postfix)
                         )
            )
    setattr(process,"patMuonIsolationSequence"+postfix,
            cms.Sequence(getattr(process,"patMuonIsoDepositsSequence"+postfix) +
                         getattr(process,"patMuonIsolationFromDepositsSequence"+postfix)
                         )
            )
    
    getattr(process,"isoDepMuonWithCharged"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllChargedCandidates"),
        DR_Veto = cms.double(1e-05),
        DepositLabel = cms.untracked.string('')
        )
    getattr(process,"isoDepMuonWithNeutral"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllNeutralHadrons"),
        DR_Veto = cms.double(0.08),
        DepositLabel = cms.untracked.string('')
        )
    getattr(process,"isoDepMuonWithPhotons"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllPhotons"),
        DR_Veto = cms.double(0.05),
        DepositLabel = cms.untracked.string('')
        )
    
    getattr(process,"isoValMuonWithPhotons"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepMuonWithPhotons"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(1.0)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValMuonWithNeutral"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepMuonWithNeutral"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(1.0)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValMuonWithCharged"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepMuonWithCharged"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(0.5)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValMuonWithChargedPU"+postfix).deposits[0].vetos = getattr(
        process,"isoValMuonWithCharged"+postfix).deposits[0].vetos

    

    module.isoDeposits = cms.PSet(
        pfAllParticles = cms.InputTag("isoDepMuonWithChargedPU"+postfix),
        pfChargedHadrons = cms.InputTag("isoDepMuonWithCharged"+postfix),
        pfNeutralHadrons = cms.InputTag("isoDepMuonWithNeutral"+postfix),
        pfPhotons = cms.InputTag("isoDepMuonWithPhotons"+postfix)
        )
    module.isolationValues = cms.PSet(
        pfAllParticles = cms.InputTag("isoValMuonWithChargedPU"+postfix),
        pfChargedHadrons = cms.InputTag("isoValMuonWithCharged"+postfix),
        pfNeutralHadrons = cms.InputTag("isoValMuonWithNeutral"+postfix),
        pfPhotons = cms.InputTag("isoValMuonWithPhotons"+postfix)
        )
    
    process.patDefaultSequence.replace(module,
                                       getattr(process,"patMuonIsolationSequence"+postfix)+
                                       module
                                       )
    process.patDefaultSequence.replace(process.isoDepMuonWithCharged,
                                       process.pfCandidateSelectionByType+
                                       process.isoDepMuonWithCharged)


####################################################################
def addPFMuon(process,postfix,verbose=False):
    if verbose:
        print "[Info] Adding pf-muon collection with postfix '"+postfix+"'"
    cloneProcessingSnippet(process, process.makePatMuons, postfix)
    getattr(process,"patMuons"+postfix).useParticleFlow = True
    pfSelectedMuons = cms.EDFilter( #dummy selector
        "PtMinPFCandidateSelector",
        src = cms.InputTag('pfAllMuons'),
        ptMin = cms.double(0.0)
        )
    setattr(process,"pfSelectedMuons"+postfix,pfSelectedMuons.clone())
    getattr(process,"patMuons"+postfix).pfMuonSource = "pfSelectedMuons"+postfix
    if  hasattr(process,"muonMatch"+postfix):
        getattr(process,"muonMatch"+postfix).src = getattr(process,"patMuons"+postfix).pfMuonSource
    ## Add pfMuons to sequence
    process.patCandidates.replace(process.makePatMuons,
                                  process.makePatMuons+
                                  getattr(process,"pfSelectedMuons"+postfix)*
                                  getattr(process,"makePatMuons"+postfix)
        )
    process.patCandidateSummary.candidates.append(cms.InputTag("patMuons"+postfix))
    # check if there is pf-isolation, if not add it
    if not hasattr(process, "patMuonIsolationFromDepositsSequence"+postfix):
        addPFMuonIsolation(process,getattr(process,"patMuons"+postfix),
                           postfix=postfix,verbose=verbose)
    #setup the isolation
    getattr(process,"isoDepMuonWithCharged"+postfix).src = getattr(process,"pfSelectedMuons"+postfix).src
    getattr(process,"isoDepMuonWithNeutral"+postfix).src = getattr(process,"pfSelectedMuons"+postfix).src
    getattr(process,"isoDepMuonWithPhotons"+postfix).src = getattr(process,"pfSelectedMuons"+postfix).src
    # and now selected Muons
    setattr(process,"selectedPatMuons"+postfix,process.selectedPatMuons.clone())
    getattr(process,"selectedPatMuons"+postfix).src = 'patMuons'+postfix
    process.selectedPatCandidates.replace(process.selectedPatMuons,
                                          process.selectedPatMuons+
                                          getattr(process,"selectedPatMuons"+postfix)
                                          )
    process.selectedPatCandidateSummary.candidates.append(cms.InputTag("selectedPatMuons"+postfix))
    # and add counter
    setattr(process,"countPatMuons"+postfix,process.countPatMuons.clone())
    getattr(process,"countPatMuons"+postfix).src = 'selectedPatMuons'+postfix
    process.countPatCandidates.replace(process.countPatMuons,
                                       process.countPatMuons+
                                       getattr(process,"countPatMuons"+postfix)
                                       )

###################a#################################################
def addPFElectronIsolation(process,module,postfix="",verbose=False):
    if verbose:
        print "[Info] Adding particle isolation to electron with postfix '"+postfix+"'"

    #if not hasattr(process, "pfCandidateSelectionByType"):
    #    addSelectedPFlowParticle(process,verbose=verbose)
        
    #setup correct src of isolated object
    setattr(process,"isoDepElectronWithCharged"+postfix,
            isoDepositReplace(module.electronSource,
                              'pfAllChargedCandidates'))
    setattr(process,"isoDepElectronWithChargedPU"+postfix,
            isoDepositReplace(module.electronSource,
                              'pfAllChargedHadronsPU'))
    setattr(process,"isoDepElectronWithNeutral"+postfix,
            isoDepositReplace(module.electronSource,
                              'pfAllNeutralHadrons'))
    setattr(process,"isoDepElectronWithPhotons"+postfix,
            isoDepositReplace(module.electronSource,
                              'pfAllPhotons'))

    #compute isolation values form deposits
    process.load("CommonTools.ParticleFlow.Isolation.pfElectronIsolationFromDeposits_cff")
    setattr(process,"isoValElectronWithChargedPU",
            process.isoValElectronWithCharged.clone())
    getattr(process,"isoValElectronWithChargedPU").deposits[0].src="isoDepElectronWithChargedPU"
    
    #compute isolation values form deposits
    process.load("CommonTools.ParticleFlow.Isolation.pfElectronIsolationFromDeposits_cff")
    if postfix!="":
        setattr(process,"isoValElectronWithCharged"+postfix,
                process.isoValElectronWithCharged.clone())
        getattr(process,"isoValElectronWithCharged"+postfix).deposits[0].src="isoDepElectronWithCharged"+postfix
        setattr(process,"isoValElectronWithChargedPU"+postfix,
                process.isoValElectronWithChargedPU.clone())
        getattr(process,"isoValElectronWithChargedPU"+postfix).deposits[0].src="isoDepElectronWithChargedPU"+postfix,
     
        setattr(process,"isoValElectronWithNeutral"+postfix,
                process.isoValElectronWithNeutral.clone())
        getattr(process,"isoValElectronWithNeutral"+postfix).deposits[0].src="isoDepElectronWithNeutral"+postfix
        setattr(process,"isoValElectronWithPhotons"+postfix,
                process.isoValElectronWithPhotons.clone())
        getattr(process,"isoValElectronWithPhotons"+postfix).deposits[0].src="isoDepElectronWithPhotons"+postfix
        
    setattr(process,"patElectronIsolationFromDepositsSequence"+postfix,
            cms.Sequence(getattr(process,"isoValElectronWithCharged"+postfix) +
                         getattr(process,"isoValElectronWithChargedPU"+postfix) +
                         getattr(process,"isoValElectronWithNeutral"+postfix) +
                         getattr(process,"isoValElectronWithPhotons"+postfix)
                         )
            )

    setattr(process,"patElectronIsoDepositsSequence"+postfix,
            cms.Sequence(getattr(process,"isoDepElectronWithCharged"+postfix) +
                         getattr(process,"isoDepElectronWithChargedPU"+postfix) +
                         getattr(process,"isoDepElectronWithNeutral"+postfix) +
                         getattr(process,"isoDepElectronWithPhotons"+postfix)
                         )
            )
    setattr(process,"patElectronIsolationSequence"+postfix,
            cms.Sequence(getattr(process,"patElectronIsoDepositsSequence"+postfix) +
                         getattr(process,"patElectronIsolationFromDepositsSequence"+postfix)
                         )
            )

    getattr(process,"isoDepElectronWithCharged"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllChargedCandidates"),
        DR_Veto = cms.double(1e-05),
        DepositLabel = cms.untracked.string('')
        )
    getattr(process,"isoDepElectronWithNeutral"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllNeutralHadrons"),
        DR_Veto = cms.double(0.08),
        DepositLabel = cms.untracked.string('')
        )
    getattr(process,"isoDepElectronWithPhotons"+postfix).ExtractorPSet = cms.PSet(
        Diff_z = cms.double(99999.99),
        ComponentName = cms.string('CandViewExtractor'),
        DR_Max = cms.double(1.0),
        Diff_r = cms.double(99999.99),
        inputCandView = cms.InputTag("pfAllPhotons"),
        DR_Veto = cms.double(0.05),
        DepositLabel = cms.untracked.string('')
        )
    
    getattr(process,"isoValElectronWithPhotons"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepElectronWithPhotons"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(1.0)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValElectronWithNeutral"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepElectronWithNeutral"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(1.0)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValElectronWithCharged"+postfix).deposits = cms.VPSet(
        cms.PSet(
        src = cms.InputTag("isoDepElectronWithCharged"+postfix),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('Threshold(0.5)'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
        )
        )
    getattr(process,"isoValElectronWithChargedPU"+postfix).deposits[0].vetos = getattr(
        process,"isoValElectronWithCharged"+postfix).deposits[0].vetos

    module.isoDeposits = cms.PSet(
        pfAllParticles = cms.InputTag("isoDepElectronWithChargedPU"+postfix),
        pfChargedHadrons = cms.InputTag("isoDepElectronWithCharged"+postfix),
        pfNeutralHadrons = cms.InputTag("isoDepElectronWithNeutral"+postfix),
        pfPhotons = cms.InputTag("isoDepElectronWithPhotons"+postfix)
        )
    module.isolationValues = cms.PSet(
        pfAllParticles = cms.InputTag("isoValElectronWithChargedPU"+postfix),
        pfChargedHadrons = cms.InputTag("isoValElectronWithCharged"+postfix),
        pfNeutralHadrons = cms.InputTag("isoValElectronWithNeutral"+postfix),
        pfPhotons = cms.InputTag("isoValElectronWithPhotons"+postfix)
        )
    
    process.patDefaultSequence.replace(module,
                                       getattr(process,"patElectronIsolationSequence"+postfix)+
                                       module
                                       )

    process.patDefaultSequence.replace(process.isoDepElectronWithCharged,
                                       process.pfCandidateSelectionByType+
                                       process.isoDepElectronWithCharged)

####################################################################
def addPFElectron(process,postfix,verbose=False):
    if verbose:
        print "[Info] Adding pf-electron collection with postfix '"+postfix+"'"
    cloneProcessingSnippet(process, process.makePatElectrons, postfix)
    getattr(process,"patElectrons"+postfix).useParticleFlow = True
    pfSelectedElectrons = cms.EDFilter( #dummy selector
        "PtMinPFCandidateSelector",
        src = cms.InputTag('pfAllElectrons'),
        ptMin = cms.double(0.0)
        )
    setattr(process,"pfSelectedElectrons"+postfix,pfSelectedElectrons.clone())
    getattr(process,"patElectrons"+postfix).pfElectronSource = "pfSelectedElectrons"+postfix
    ## Add pfElectrons to sequence
    process.patCandidates.replace(process.makePatElectrons,
                                  process.makePatElectrons+
                                  getattr(process,"pfSelectedElectrons"+postfix)*
                                  getattr(process,"makePatElectrons"+postfix)
        )
    process.patCandidateSummary.candidates.append(cms.InputTag("patElectrons"+postfix))
    # check if there is pf-isolation, if not add it
    if not hasattr(process, "patElectronIsolationFromDepositsSequence"+postfix):
        addPFElectronIsolation(process,getattr(process,"patElectrons"+postfix),
                           postfix=postfix,verbose=verbose)
    #setup the isolation
    getattr(process,"isoDepElectronWithCharged"+postfix).src = getattr(process,"pfSelectedElectrons"+postfix).src
    getattr(process,"isoDepElectronWithNeutral"+postfix).src = getattr(process,"pfSelectedElectrons"+postfix).src
    getattr(process,"isoDepElectronWithPhotons"+postfix).src = getattr(process,"pfSelectedElectrons"+postfix).src
    # and now selected Electrons
    setattr(process,"selectedPatElectrons"+postfix,process.selectedPatElectrons.clone())
    getattr(process,"selectedPatElectrons"+postfix).src = 'patElectrons'+postfix
    process.selectedPatCandidates.replace(process.selectedPatElectrons,
                                          process.selectedPatElectrons+
                                          getattr(process,"selectedPatElectrons"+postfix)
                                          )
    process.selectedPatCandidateSummary.candidates.append(cms.InputTag("selectedPatElectrons"+postfix))
    # and add counter
    setattr(process,"countPatElectrons"+postfix,process.countPatElectrons.clone())
    getattr(process,"countPatElectrons"+postfix).src = 'selectedPatElectrons'+postfix
    process.countPatCandidates.replace(process.countPatElectrons,
                                       process.countPatElectrons+
                                       getattr(process,"countPatElectrons"+postfix)
                                       )


    
####################################################################
def addTriggerMatchingMuon(process,isMC=False,postfix="",verbose=False):
    if verbose:
        print "[Info] Addting trigger matching to patMuon with postfix '"+postfix+"'"
    
    process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
    muonMatch = cms.EDProducer(
        "PATTriggerMatcherDRLessByR"
        , src     = cms.InputTag( "selectedPatMuons"+postfix )
        , matched = cms.InputTag( "patTrigger" )
        , matchedCuts    = cms.string('(path("HLT_IsoMu12_LooseIsoPFTau10_v*",0,0) || path("HLT_IsoMu15_LooseIsoPFTau15_v*",0,0) || path("HLT_IsoMu15_LooseIsoPFTau20_v*",0,0) ) && (filter("hltSingleMuIsoL3IsoFiltered15") || filter("hltSingleMuIsoL3IsoFiltered12")) && type("TriggerMuon")')
       #, matchedCuts    = cms.string('path("HLT_Mu11_PFTau15_v*",0) && filter("hltSingleMu11L3Filtered11") && type("TriggerMuon")')
        , maxDPtRel = cms.double( 999 )
        , maxDeltaR = cms.double( 0.3 )
        , resolveAmbiguities    = cms.bool( True )
        , resolveByMatchQuality = cms.bool( True )
        )
    if isMC:
        muonMatch.matchedCuts  = cms.string('(path("HLT_Mu15_LooseIsoPFTau20_v*",0,0) || path("HLT_IsoMu12_LooseIsoPFTau10_v*",0,0) ) && (filter("hltL3Muon15") || filter("hltSingleMuIsoL3IsoFiltered12") ) && type("TriggerMuon")')
        

    setattr(process,"muonTriggerMatchHLTMuons"+postfix,muonMatch.clone()) 
    if not hasattr(process,"patTriggerSequence"):
        setattr(process,"patTriggerSequence",
                cms.Sequence(process.patTrigger))
        process.patDefaultSequence += process.patTriggerSequence

    if not hasattr(process,"patTriggerMatcher"):
        setattr(process,"patTriggerMatcher",
                cms.Sequence(getattr(process,"muonTriggerMatchHLTMuons"+postfix)))
        process.patTriggerSequence += process.patTriggerMatcher
    else:
        process.patTriggerMatcher += getattr(process,"muonTriggerMatchHLTMuons"+postfix)

    setattr(process,"selectedPatMuonsTriggerMatch"+postfix,
            cms.EDProducer("PATTriggerMatchMuonEmbedder",
                           src = cms.InputTag( "selectedPatMuons"+postfix ),
                           matches = cms.VInputTag("muonTriggerMatchHLTMuons"+postfix)
                           ))
    if not hasattr(process,"patTriggerMatchEmbedder"):
        setattr(process,"patTriggerMatchEmbedder",
                cms.Sequence(getattr(process,"selectedPatMuonsTriggerMatch"+postfix)))
        process.patTriggerSequence += process.patTriggerMatchEmbedder
    else:
        process.patTriggerMatchEmbedder += getattr(process,"selectedPatMuonsTriggerMatch"+postfix)
    

    process.patTrigger.onlyStandAlone = True


####################################################################
def addTriggerMatchingElectron(process,isMC=False,postfix="",verbose=False):
    if verbose:
        print "[Info] Addting trigger matching to patElectron with postfix '"+postfix+"'"
    
    process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
    eleMatch = cms.EDProducer(
        "PATTriggerMatcherDRLessByR"
        , src     = cms.InputTag( "selectedPatElectrons"+postfix )
        , matched = cms.InputTag( "patTrigger" )
        #, matchedCuts = cms.string( 'path("HLT_IsoEle12_PFTau15_v3",0) && type("TriggerElectron")' )
        #, matchedCuts = cms.string( 'type("TriggerElectron")' )
        #, matchedCuts = cms.string( 'type("TriggerTau")' )
        , matchedCuts = cms.string('(path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*", 0,0) || path("HLT_Ele18_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*",0,0)) && (filter("hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter") || filter("hltEle18CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter")) && type("TriggerElectron")')
        , maxDPtRel = cms.double( 999 )
        , maxDeltaR = cms.double( 0.3)
        , resolveAmbiguities    = cms.bool( True )
        , resolveByMatchQuality = cms.bool( True )
        )

    if isMC:
        eleMatch.matchedCuts = cms.string('( (path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v*",0,0)) || (path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*",0,0)) ) && filter("hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter") && filter("hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter") && type("TriggerElectron")')

    setattr(process,"eleTriggerMatchHLTElectrons"+postfix,eleMatch.clone()) 
    if not hasattr(process,"patTriggerSequence"):
        setattr(process,"patTriggerSequence",
                cms.Sequence(process.patTrigger))
        process.patDefaultSequence += process.patTriggerSequence

    if not hasattr(process,"patTriggerMatcher"):
        setattr(process,"patTriggerMatcher",
                cms.Sequence(getattr(process,"eleTriggerMatchHLTElectrons"+postfix)))
        process.patTriggerSequence += process.patTriggerMatcher
    else:
        process.patTriggerMatcher += getattr(process,"eleTriggerMatchHLTElectrons"+postfix)

    setattr(process,"selectedPatElectronsTriggerMatch"+postfix,
            cms.EDProducer("PATTriggerMatchElectronEmbedder",
                           src = cms.InputTag( "selectedPatElectrons"+postfix ),
                           matches = cms.VInputTag("eleTriggerMatchHLTElectrons"+postfix)
                           ))
    if not hasattr(process,"patTriggerMatchEmbedder"):
        setattr(process,"patTriggerMatchEmbedder",
                cms.Sequence(getattr(process,"selectedPatElectronsTriggerMatch"+postfix)))
        process.patTriggerSequence += process.patTriggerMatchEmbedder
    else:
        process.patTriggerMatchEmbedder += getattr(process,"selectedPatElectronsTriggerMatch"+postfix)
    

    process.patTrigger.onlyStandAlone = True


####################################################################
def addTriggerMatchingTau(process,isMC=False,postfix="",XtriggerMu=False,verbose=False):
    if verbose:
        print "[Info] Addting trigger matching to patTau with postfix '"+postfix+"'"
    
    process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
    tauMatch = cms.EDProducer(
        "PATTriggerMatcherDRLessByR"
        , src     = cms.InputTag( "selectedPatTaus"+postfix )
        , matched = cms.InputTag( "patTrigger" )
        , matchedCuts    = cms.string('(path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*", 0,0) || path("HLT_Ele18_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*",0,0)) && filter("hltPFTau20TrackLooseIso") && type("TriggerTau")')
        , maxDPtRel = cms.double( 999 )
        , maxDeltaR = cms.double( 0.3 )
        , resolveAmbiguities    = cms.bool( True )
        , resolveByMatchQuality = cms.bool( True )
        )


    if XtriggerMu and (not isMC):
        tauMatch.matchedCuts  = cms.string('( path("HLT_IsoMu12_LooseIsoPFTau10_v*",0,0) || path("HLT_IsoMu15_LooseIsoPFTau15_v*",0,0) || path("HLT_IsoMu15_LooseIsoPFTau20_v*",0,0) ) && ( filter("hltPFTau20TrackLooseIso") ||  filter("hltPFTau15TrackLooseIso") ) && type("TriggerTau")')

    if XtriggerMu and isMC:
         tauMatch.matchedCuts  = cms.string('(path("HLT_Mu15_LooseIsoPFTau20_v*",0,0) || path("HLT_IsoMu12_LooseIsoPFTau10_v*",0,0)) && (filter("hltPFTau20TrackLooseIso") ||  filter("hltPFTau10TrackLooseIso") ) && type("TriggerTau")')
 
    if (not XtriggerMu) and isMC:
        tauMatch.matchedCuts  = cms.string('(path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v*", 0,0) || path("HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v*",0,0))  && filter("hltPFTau20TrackLooseIso") && type("TriggerTau")')
        

    setattr(process,"tauTriggerMatchHLTTaus"+postfix,tauMatch.clone()) 
    if not hasattr(process,"patTriggerSequence"):
        setattr(process,"patTriggerSequence",
                cms.Sequence(process.patTrigger))
        process.patDefaultSequence += process.patTriggerSequence

    if not hasattr(process,"patTriggerMatcher"):
        setattr(process,"patTriggerMatcher",
                cms.Sequence(getattr(process,"tauTriggerMatchHLTTaus"+postfix)))
        process.patTriggerSequence += process.patTriggerMatcher
    else:
        process.patTriggerMatcher += getattr(process,"tauTriggerMatchHLTTaus"+postfix)

    setattr(process,"selectedPatTausTriggerMatch"+postfix,
            cms.EDProducer("PATTriggerMatchTauEmbedder",
                           src = cms.InputTag( "selectedPatTaus"+postfix ),
                           matches = cms.VInputTag("tauTriggerMatchHLTTaus"+postfix)
                           ))
    if not hasattr(process,"patTriggerMatchEmbedder"):
        setattr(process,"patTriggerMatchEmbedder",
                cms.Sequence(getattr(process,"selectedPatTausTriggerMatch"+postfix)))
        process.patTriggerSequence += process.patTriggerMatchEmbedder
    else:
        process.patTriggerMatchEmbedder += getattr(process,"selectedPatTausTriggerMatch"+postfix)
    

    process.patTrigger.onlyStandAlone = True
#################################################################################
