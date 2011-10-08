import FWCore.ParameterSet.Config as cms

####################################################################
def setupEleTrigger(process, hltProcessName ):

    process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff")
    process.patTrigger.processName = hltProcessName

    process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi")
    process.electronTriggerMatchHLTEle15LWL1R.src = "selectedPatElectrons"

    process.electronTriggerMatchHLTEle15LWL1R.pathNames = cms.vstring()
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Photon15_Cleaned_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Photon10_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Photon10_Cleaned_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Ele10_LW_EleId_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Ele10_SW_EleId_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Ele12_SW_TightEleIdIsol_L1R' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Ele12_SW_TighterEleIdIsol_L1R_v1' )
    process.electronTriggerMatchHLTEle15LWL1R.pathNames.append('HLT_Ele17_SW_CaloEleId_L1R')
    
    process.electronTriggerMatchHLTEle15LWL1R.maxDeltaR = 0.5
    process.electronTriggerMatchHLTEle15LWL1R.maxDPtRel = 999.
    
    process.patTriggerMatcher += process.electronTriggerMatchHLTEle15LWL1R
    process.patTriggerMatcher.remove( process.patTriggerMatcherElectron )
    process.patTriggerMatcher.remove( process.patTriggerMatcherMuon )
    process.patTriggerMatcher.remove( process.patTriggerMatcherTau )

    process.patElectronsTriggerMatchEmbedder = cms.EDProducer("PATTriggerMatchElectronEmbedder",
                                                          src = cms.InputTag( "selectedPatElectrons" ),
                                                          matches = cms.VInputTag( "electronTriggerMatchHLTEle15LWL1R" )
                                                          )

    process.patTriggerSequence += process.patElectronsTriggerMatchEmbedder

    from PhysicsTools.PatAlgos.tools.trigTools import switchOnTriggerStandAlone
    switchOnTriggerStandAlone( process )

####################################################################
####################################################################
    
def addCutBasedID( process ):

    #ele ID only
    #process.load("RecoEgamma.ElectronIdentification.electronIdCutBasedClassBasedExt_cfi")
    #process.eidClassBased95 = process.eidCutBasedClassBasedExt.clone()
    #process.eidClassBased95.electronQuality = cms.string('Eff95Cuts')
    #process.eidClassBased90 = process.eidCutBasedClassBasedExt.clone()
    #process.eidClassBased90.electronQuality = cms.string('Eff90Cuts')

    #ele ID + ISO + Conv Removal
    process.load("LLRAnalysis.Utilities.simpleEleIdSequence_cff")

    #process.load("RecoEgamma.ElectronIdentification.likelihoodPdfsDB_cfi")
    #process.load("RecoEgamma.ElectronIdentification.likelihoodESetup_cfi")

    #from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import eidLikelihoodExt
    #process.egammaIDLikelihood   = eidLikelihoodExt.clone()
    #process.electronIDLHSequence = cms.Sequence( process.egammaIDLikelihood )


    process.patElectronId = cms.Sequence(
        process.simpleEleIdSequence #+
        #process.electronIDLHSequence
        #process.eidClassBased95 +
        #process.eidClassBased90
        )

    process.patElectrons.electronIDSources = cms.PSet(
        
        simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
        simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
        simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
        simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
        simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
        simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
        simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
        simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
        simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
        simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
        simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
        simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),
        #electronIDLH = cms.InputTag("egammaIDLikelihood"),
        #eidLoose = cms.InputTag("eidClassBased95"),
        #eidTight = cms.InputTag("eidClassBased90")
        )


    #process.makePatElectrons.replace(process.patElectrons,
    #                                 process.patElectronId*
    #                                 process.patElectrons
    #                                 )
    process.patDefaultSequence.replace(process.patElectrons,
                                       process.patElectronId*
                                       process.patElectrons
                                       )
    
    
    #process.patElectrons.embedTrack = True
    process.electronMatch.maxDeltaR = 0.5
    process.electronMatch.maxDPtRel = 0.5

####################################################################
####################################################################

    

