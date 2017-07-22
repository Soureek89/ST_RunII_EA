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
                 [#'file:/tmp/oiorio/B2GEDMNtuple_1.root'
#                 'file:/afs/cern.ch/user/s/smitra/work/scratch0/SingleTop/13TeV/Test/CMSSW_7_6_3_patch2/src/Analysis/B2GAnaFW/test/B2GEDMNtuple.root'
#'root://xrootd.ba.infn.it//store/user/decosa/ttDM/Phys14_v2/TTDMDMJets_M600GeV_Tune4C_13TeV-madgraph-tauola/DM600_Phys14DR-PU20bx25_PHYS14_25_V1-v1_EDMNtuple/150212_173740/0000/B2GEDMNtuple_5.root', 
#'root://xrootd.ba.infn.it//store/user/decosa/ttDM/Phys14_v2/TTDMDMJets_M600GeV_Tune4C_13TeV-madgraph-tauola/DM600_Phys14DR-PU20bx25_PHYS14_25_V1-v1_EDMNtuple/150212_173740/0000/B2GEDMNtuple_4.root'],
#				'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/EDMTuple_80X/SingleMuon/SingleMu_2016B_PromptReco-v2/160718_110537/0000/SingleMu_2016B_EDMTuple_999.root'	
],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('outputLabel',
                 'treesTest_NewSmall_Data.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.register('isData',
                 True,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Is data?')

options.register('useLHE',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Keep LHEProducts')

options.register('lhes',
#                 'source',
                 'externalLHEProducer',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'name from generator')

options.parseArguments()

if(options.isData):
	options.useLHE = False

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
#        options.sample
        )
)

#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag.globaltag = options.globalTag 


#from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:startup_GRun')
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc_50nsGRun')
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = '74X_dataRun2_Prompt_v0')
#process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
#process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

#process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v0'
#process.GlobalTag.globaltag = '76X_dataRun2_v15'
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'

#for pset in process.GlobalTag.toGet.value():
#    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#    #   Fix for multi-run processing:
#    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
#    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
    

### Rootplizer

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputLabel))
process.load("TreeMaker.ST_RunII_EA.topplusdmedmRootTreeMaker_cff")
#process.DMTreesDumper.lhes =cms.InputTag("externalLHEProducer")
#if(options.useLHE):

#process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
#process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
#process.DMTreesDumper.doPU= cms.bool(False)
#process.DMTreesDumper.dataPUFile=cms.string("DistrFall15_25ns")


process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
#process.DMTreesDumper.systematics =(options.syst)
process.DMTreesDumper.changeJECs = cms.untracked.bool(False)# JEC via GT
process.DMTreesDumper.useMETNoHF = cms.untracked.bool(False)
process.DMTreesDumper.addPV = cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.useLHEWeights =cms.untracked.bool(False)
process.DMTreesDumper.isData = cms.untracked.bool(True)#This adds the L2L3Residuals
process.DMTreesDumper.doPU= cms.bool(False)
process.DMTreesDumper.dataPUFile=cms.string("DistrSpring16_25ns")                                                                                                                


process.analysisPath = cms.Path(
    process.DMTreesDumper
    )
