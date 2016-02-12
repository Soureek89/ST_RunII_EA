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

options.register('skip',
                 0,# default value: process all events
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.int,
                 'Number of events to skip')


options.register('sample',
                 '/ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/paktinat-B2GAnaFW_7415-1aae21629c396d43830e04018539aad4/USER',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('sourcedir',
                 'gsiftp://se1.particles.ipm.ac.ir/dpm/particles.ipm.ac.ir/home/cms/' , 
                 #'srm://se1.particles.ipm.ac.ir:8446/srm/managerv2?SFN=/dpm/particles.ipm.ac.ir/home/cms/',
                 #'rfio:///dpm/particles.ipm.ac.ir/home/cms/',
                 #'file:/home/hbakhshi/mnt/',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'source prefix')

options.register('destination',
                 '/cmsdata2/hbakhshi/tchannel25ns',
                 #'rfio:///dpm/particles.ipm.ac.ir/home/cms/',
                 #'file:/home/hbakhshi/mnt/',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'source prefix')

options.register('outputLabel',
                 'treesTest_NewSmall.root',
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

options.register('lhes',
#                 'source',
                 'externalLHEProducer',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'name from generator')

options.register('syst',
                 ['noSyst'],
#                 ['noSyst','jer__up','jer__down','jes__up','jes__down','unclusteredMet__up','unclusteredMet__down'],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'systematic trees')


options.register('globalTag',
                 '74X_mcRun2_asymptotic_v2',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'global tag to be used')

options.parseArguments()

if(options.isData):options.useLHE = False

process = cms.Process("ttDManalysisTrees")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.categories.append('HLTrigReport')
### Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
### Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )
### Source file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(

        ),
                            skipEvents=cms.untracked.uint32(options.skip)

)



from das_client import *
jsondict = get_data( "https://cmsweb.cern.ch" , 
                     "file dataset=%(sample)s instance=prod/phys03"  %  {'sample':options.sample} ,
                     0 , #idx
                     0 , #limit
                     0 , #verbose
                     300 , #waiting time
                     "" ,  #ckey
                     "" , #cert
                     )
cli_msg  = jsondict.get('client_message', None)
if  cli_msg:
    print "DAS CLIENT WARNING: %s" % cli_msg
if  'status' not in jsondict:
    print 'DAS record without status field:\n%s' % jsondict
    sys.exit(EX_PROTOCOL)
if  jsondict['status'] != 'ok':
    print "status: %s, reason: %s" \
        % (jsondict.get('status'), jsondict.get('reason', 'N/A'))
    sys.exit(EX_TEMPFAIL)

data = jsondict['data']

import os, ntpath
from subprocess import call

try:
    os.makedirs( options.destination + options.sample )
except os.error :
    print "directory already exits"

iii = 1
for jjj in data:
    print "%d/%d : %s" % ( iii , len(data) , ntpath.basename(str(prim_value(jjj))) )
    iii += 1
    sourcefile = options.sourcedir + str(prim_value( jjj ))
    sourcesize = jjj['file'][0]['size']
    destinationfile =  options.destination + options.sample +'/' + ntpath.basename(str(prim_value(jjj)) )
    process.source.fileNames.append( 'file:' + destinationfile )
    destsize = -1
    if os.path.exists( destinationfile ):
        destsize = os.path.getsize(destinationfile)
    
    if destsize == sourcesize :
        print "\t already exists" 
        continue

    if  destsize > 0 :
        print "\t is being removed" 
        os.remove( destinationfile )

    command = [ 'gfal-copy' , sourcefile , 'file:' + destinationfile ]
    print command
    print "\t started to copy"
    call( command )
    
    #print command 

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

try:
    os.makedirs( options.destination + "/Trees/" )
except os.error :
    print "directory already exits"



if not options.skip == 0 :
    options.outputLabel = options.outputLabel.replace( ".root" ,  "_from" + str(options.skip)  + ".root" )

process.TFileService = cms.Service("TFileService", fileName = cms.string( options.destination + "/Trees/" + options.outputLabel))
process.load("Analysis.ST_RunII_EA.topplusdmedmRootTreeMaker_cff")
#process.DMTreesDumper.lhes =cms.InputTag("externalLHEProducer")
process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
process.DMTreesDumper.systematics =(options.syst)
process.DMTreesDumper.changeJECs = cms.untracked.bool(False)# JEC via GT
process.DMTreesDumper.useMETNoHF = cms.untracked.bool(False)
#process.DMTreesDumper.addPV = cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.useLHEWeights =cms.untracked.bool(False)
process.DMTreesDumper.isData = cms.untracked.bool(False)#This adds the L2L3Residuals
process.DMTreesDumper.doPU= cms.bool(True);
process.DMTreesDumper.dataPUFile=cms.string("DistrFall15_25ns");

process.analysisPath = cms.Path(
    process.DMTreesDumper
    )


