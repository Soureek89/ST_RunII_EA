### *****************************************************************************************
### Usage:
###
### cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=N sample="mySample/sample.root" version="71" outputLabel="myoutput"
###
### Default values for the options are set:
### maxEvts     = -1
### sample      = 'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root'
### outputLabel = 'analysisTTDM.root'
### *****************************************************************************************
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts



options = opts.VarParsing ('analysis')

options.register('maxEvts',
                 -1,# default value: process all events
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.int,
                 'Number of events to process')

options.register('sample',
				[
#				'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/EDMTuple_80X/ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/TChannel_Powheg_EDMTuple/160714_102438/0000/TChannel_aMCatNLO_EDMTuple_9.root'
				'file:B2GEDMNtuple_tChannel.root'
#				'file:B2GEDMNtuple_QCD.root'
#				'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/2017/EDMTuple_80X/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/DYJets_aMCatNLO_EDMTuple_Summer16/170411_132805/0000/DYJets_aMCatNLO_EDMTuple_97.root'
#				'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/2017/EDMTuple_80X/ST_t-channel_top_4f_mtop1785_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/TChannel_Mass178p5_EDMTuple_Summer16/170531_133128/0000/TChannel_Mass178p5_EDMTuple_12.root'	
				],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('outputLabel',
                 'TChannel_Syst_Trial.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.register('isData',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Is data?')

options.register('useLHE',
                 True,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Keep LHEProducts')

options.register('topPtReweight',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Do top Pt Reweighting')
                  	
options.register('lhes',
#                 'source',
                 'externalLHEProducer',	
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'name from generator')

options.register('syst',
#                 ['noSyst'],
#                  ['jer__up','jer__down','jes_CorrelationGroupUncorrelated_up','jes_CorrelationGroupUncorrelated_down','jes_CorrelationGroupMPFInSitu_up',"jes_CorrelationGroupMPFInSitu_down","jes_CorrelationGroupIntercalibration_up","jes_CorrelationGroupIntercalibration_down","jes_CorrelationGroupFlavor_up","jes_CorrelationGroupFlavor_down","jes_CorrelationGroupbJES_up","jes_CorrelationGroupbJES_down","jes_FlavorPureQuark_up","jes_FlavorPureQuark_down","jes_FlavorPureGluon_up","jes_FlavorPureGluon_down","jes_FlavorPureCharm_up","jes_FlavorPureCharm_down","jes_FlavorPureBottom_up","jes_FlavorPureBottom_down","jes_SubTotalPileUp_up","jes_SubTotalPileUp_down","jes_Total_up","jes_Total_down"],
                 ['jer__up','jer__down','jes_CorrelationGroupUncorrelated_up','jes_CorrelationGroupUncorrelated_down','jes_CorrelationGroupMPFInSitu_up',"jes_CorrelationGroupMPFInSitu_down","jes_CorrelationGroupIntercalibration_up","jes_CorrelationGroupIntercalibration_down","jes_CorrelationGroupFlavor_up","jes_CorrelationGroupFlavor_down","jes_FlavorPureQuark_up","jes_FlavorPureQuark_down","jes_FlavorPureGluon_up","jes_FlavorPureGluon_down","jes_FlavorPureCharm_up","jes_FlavorPureCharm_down","jes_FlavorPureBottom_up","jes_FlavorPureBottom_down","jes_SubTotalPileUp_up","jes_SubTotalPileUp_down"],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'systematic trees')


options.register('globalTag',
#                 '76X_mcRun2_asymptotic_v12',
                 #'76X_dataRun2_v15',
# 				 '80X_mcRun2_asymptotic_2016_miniAODv2',	
				 '80X_mcRun2_asymptotic_2016_TrancheIV_v8',	
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'global tag to be used')

options.parseArguments()

if(options.isData):options.useLHE = False

process = cms.Process("ttDManalysisTrees")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.append('HLTrigReport')
### Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
### Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )
### Source file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
#          options.sample
        )
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
print "# using GT ", options.globalTag
process.GlobalTag.globaltag = options.globalTag

#from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc_50nsGRun')
#process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
#process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

#for pset in process.GlobalTag.toGet.value():
#    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#    #   Fix for multi-run processing:
#    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
#    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )

    

### Rootplizer

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputLabel))
process.load("TreeMaker.ST_RunII_EA.topplusdmedmRootTreeMaker_cff")
#process.DMTreesDumper.lhes =cms.InputTag("externalLHEProducer")
process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
process.DMTreesDumper.channelInfo.topPtreweight =(options.topPtReweight)
process.DMTreesDumper.systematics =(options.syst)
process.DMTreesDumper.changeJECs = cms.untracked.bool(False)# JEC via GT
process.DMTreesDumper.useMETNoHF = cms.untracked.bool(False)
#process.DMTreesDumper.addPV = cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.useLHEWeights =cms.untracked.bool(False)
process.DMTreesDumper.channelInfo.addLHAPDFWeights = cms.untracked.bool(False)
process.DMTreesDumper.isData = cms.untracked.bool(False)#This adds the L2L3Residuals
process.DMTreesDumper.doPU= cms.bool(True);
process.DMTreesDumper.dataPUFile=cms.string("DistrSummer16_25ns");

process.analysisPath = cms.Path(
    process.DMTreesDumper
)
