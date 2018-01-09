/**
 *\Class DMAnalysisTreeMaker
 * 
 * Produces analysis trees from edm-ntuples adding extra variables for resolved and unresolved tops
 * For custom systematics scenarios
 * 
 * \Author A. Orso M. Iorio
 * 
 * 
 *\version  $Id:
 * 
 * 
*/ 

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/GetterOfProducts.h"
#include "FWCore/Framework/interface/ProcessMatch.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include <Math/VectorUtil.h>
#include "FWCore/Framework/interface/Run.h"
#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondFormats/BTauObjects/interface/BTagEntry.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH2.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

using namespace reco;
using namespace edm;
using namespace std;


namespace LHAPDF
{
void initPDFSet(int nset, const std::string &filename, int member = 0);
int numberPDF(int nset);
void usePDFMember(int nset, int member);
double xfx(int nset, double x, double Q, int fl);
double getXmin(int nset, int member);
double getXmax(int nset, int member);
double getQ2min(int nset, int member);
double getQ2max(int nset, int member);
void extrapolate(bool extrapolate = true);
}

class  DMAnalysisTreeMaker : public edm::EDAnalyzer 
{
public:
  explicit DMAnalysisTreeMaker( const edm::ParameterSet & );   
  void beginRun(const edm::Run & , const edm::EventSetup & );   
  void endRun(const edm::Run & , const edm::EventSetup &); 
private:
  virtual void beginJob() {
    std::string distr = "pileUp" + dataPUFile_ + ".root";
    LumiWeights_ = edm::LumiReWeighting(distr,"DataPileupHistogram_69p2mbMinBias.root",std::string("pileup"),std::string("pileup"));	
    LumiWeightsUp_ = edm::LumiReWeighting(distr,"DataPileupHistogram_69p2mbMinBias_up.root",std::string("pileup"),std::string("pileup"));
    LumiWeightsDown_ = edm::LumiReWeighting(distr,"DataPileupHistogram_69p2mbMinBias_down.root",std::string("pileup"),std::string("pileup"));
  }

  virtual void analyze(const edm::Event &, const edm::EventSetup & );
  vector<string> additionalVariables(string);
  string makeName(string label,string pref,string var);
  string makeBranchName(string label, string pref, string var);
  void initializePdf(string centralpdf,string variationpdf);
  void getEventPdf();
  bool getEventTriggers();
  void getEventLHEWeights();
  bool getMETFilters();

  double getTopPtWeight(double ptT,double ptTbar, bool extrap = false);

  double jetUncertainty(double pt, double eta, string syst);
  //  double smearPt(double pt, double genpt, double eta, string syst);
  double smear(double pt, double genpt, double eta, string syst);
  double resolSF(double eta, string syst);
//  double getScaleFactor(double pt, double eta, double partonFlavour, string syst);
  double muonSF(bool isdata, float pt, float eta, int syst);
  
  //------------------ Soureek adding pile-up Info ------------------------------
  void getPUSF();
  //-----------------------------------------------------------------------------

  bool isInVector(std::vector<std::string> v, std::string s);
  std::vector<edm::ParameterSet > physObjects;
  std::vector<edm::InputTag > variablesFloat, variablesInt, singleFloat, singleInt;
  edm::EDGetTokenT< LHEEventProduct > t_lhes_;
  edm::EDGetTokenT< GenEventInfoProduct > t_genprod_;
  edm::EDGetTokenT< std::vector<reco::GenParticle>> t_genParticles_;

  edm::EDGetTokenT< std::vector<string> > t_triggerNames_;
//  edm::GetterOfProducts<YourDataType> t_triggerNames_;
  edm::EDGetTokenT< std::vector<float> > t_triggerBits_;
  edm::EDGetTokenT< std::vector<int> > t_triggerPrescales_;
  edm::EDGetTokenT< unsigned int > t_lumiBlock_;
  edm::EDGetTokenT< unsigned int > t_runNumber_;
  edm::EDGetTokenT< ULong64_t > t_eventNumber_;

  edm::EDGetTokenT< std::vector<string> > t_metNames_;
  edm::EDGetTokenT< std::vector<float> > t_metBits_;

  edm::EDGetTokenT< bool > t_HBHEFilter_;
  edm::EDGetTokenT< bool > t_HBHEIsoFilter_;

  edm::EDGetTokenT< std::vector<float> > t_pvZ_,t_pvChi2_,t_pvRho_;
  edm::EDGetTokenT< std::vector<int> > t_pvNdof_;

  edm::EDGetTokenT<double> t_Rho_;
  edm::EDGetTokenT<int> t_ntrpu_;

  //====================================

  TTree * treesBase;
  map<string, TTree * > trees;
  std::vector<string> names;
  std::vector<string> systematics;
  map< string , float[100] > vfloats_values;
  map< string , int[100] > vints_values;
  map< string , string > obs_to_obj;
  map< string , string > obj_to_pref;
  
  map< string , float > float_values;
  map< string , int > int_values;
  map< string , int > sizes;

  map< string , bool > got_label; 
  map< string , int > max_instances; 
  map< int, int > subj_jet_map;

  map<string, edm::Handle<std::vector<float> > > h_floats;
  map<string, edm::Handle<std::vector<int> > > h_ints;
  map<string, edm::Handle<float> > h_float;
  map<string, edm::Handle<int> >h_int;

  map<string, edm::EDGetTokenT< std::vector<float> >  > t_floats;
  map<string, edm::EDGetTokenT< std::vector<int> > > t_ints;
  map<string, edm::EDGetTokenT<float>  > t_float;
  map<string, edm::EDGetTokenT<int> >t_int;

  string mu_label, ele_label, jets_label, met_label, metnohf_label, jetsnohf_label;


  //MC info:
  edm::ParameterSet channelInfo;
  std::string channel;
  double crossSection, originalEvents;
  bool useLHEWeights, useLHE, useTriggers,cutOnTriggers, useMETFilters, addPV;
  bool addLHAPDFWeights;
  string centralPdfSet,variationPdfSet;
  std::vector<string> leptonicTriggers, hadronicTriggers, metFilters;
  int maxPdf, maxWeights;
  edm::Handle<GenEventInfoProduct> genprod;
  edm::Handle<std::vector<reco::GenParticle> > prunedGenParticles;
  edm::Handle<LHEEventProduct > lhes;
  edm::Handle<std::vector<float> > triggerBits;
  edm::Handle<std::vector<string> > triggerNames;
  edm::Handle<std::vector<int> > triggerPrescales;

  edm::Handle<std::vector<float> > metBits;
  edm::Handle<std::vector<string> > metNames;
//  edm::Handle<bool> HBHE;
  edm::Handle<bool> HBHE;
  edm::Handle<bool> HBHEIso;
	
  edm::Handle<unsigned int> lumiBlock;
  edm::Handle<unsigned int> runNumber;
  edm::Handle<ULong64_t> eventNumber;

  edm::InputTag metNames_, metBits_;	
  //  float pdf_weights[140];
  //float lhe_weights[20];  
  // std::string lhe_weights_id[20];  

  edm::Handle<std::vector<float> > pvZ,pvChi2,pvRho;
  edm::Handle<std::vector<int> > pvNdof;
  int nPV;

  //----------------------------- Soureek adding for PU info -------------------------
  bool doPU_;
  std::string dataPUFile_;
  std::string Era;		
  edm::Handle<int> npv, ntrpu;
  edm::Handle<std::vector<int> > pubx, puNInt; 
  int nTruePU;
  edm::LumiReWeighting LumiWeights_, LumiWeightsUp_, LumiWeightsDown_;
  float puWeight, puWeightUp, puWeightDown;
  //--------------------------------------------------------------------------------------

/// ------------------------------Gen Particle Info ---------------------------------------
  float genPartpt, genParteta, genPartphi, genPartE;
  int genPartId;
///----------------------------------------------------------------------------------------		
  //JEC info
  bool changeJECs = false;
  bool isData, useMETNoHF;
  edm::Handle<double> rho;
  double Rho;
  std::vector<double> jetScanCuts;
  std::string jes_syst;
  char singleNumeral; 
  int jNumeral;		
  std::vector<JetCorrectorParameters> jecPars;
  JetCorrectorParameters *jecParsL1, *jecParsL2, *jecParsL3, *jecParsL2L3Residuals;
  map<string, JetCorrectionUncertainty *> jecUnc;

//  JetCorrectionUncertainty *jecUnc;

 	
  FactorizedJetCorrector *jecCorr;

  bool isFirstEvent;
  
  BTagCalibration *calib_cmvav2;
  BTagCalibrationReader *readerCMVALoose,*readerCMVALooseUDSG, *readerCMVAMedium, *readerCMVATight;// *readerCMVAReshape;  

  TString filename_cmva;
  TFile* file_cmva;


  class Weights{

    public:
      Weights( TFile * file , std::string name ){
        histoFile = file;
        wHisto = (TH2F*)histoFile->Get(name.c_str());
      }
  
      float getEff(float eta, float pt){ 
        int i(-1), j(-1);
        findBin( eta, pt, i, j);
/*
        int i = wHisto->GetXaxis()->FindBin(eta);
        int j = wHisto->GetYaxis()->FindBin(pt);
        if(i == 0) i = 1;
        if(j == wHisto->GetYaxis()->GetNbins() + 1)  j =  wHisto->GetYaxis()->GetNbins();
*/
//      cout << " i "<< i << " j " << j <<" eta "<< eta << " pt "<< pt <<endl;
        float eff = wHisto->GetBinContent(i,j) ;
        return eff;
      }
 
      float getErr(float eta, float pt)
      { 
//       std::cout << "getting bin" << std::endl;
         int i(-1), j(-1);
         findBin( eta, pt, i, j);
/*
         int i = wHisto->GetXaxis()->FindBin(eta);
         int j = wHisto->GetYaxis()->FindBin(pt);
         if(i == 0)  i = 1;
         if(j == wHisto->GetYaxis()->GetNbins() + 1) j =  wHisto->GetYaxis()->GetNbins();
*/
         float err = wHisto->GetBinError(i,j) ;
         return err;
      }
 
      void findBin(float eta, float pt, int & i, int & j){ 
        i = wHisto->GetXaxis()->FindBin(eta);
        j = wHisto->GetYaxis()->FindBin(pt);
        if(i == 0) i = 1;
        if(j == wHisto->GetYaxis()->GetNbins() + 1) j =  wHisto->GetYaxis()->GetNbins();
      }
  
    private:
      TFile* histoFile;
      TH2F* wHisto;
  };

  Weights *cmvaeffbt,*cmvaeffbm,*cmvaeffbl;
  Weights *cmvaeffct,*cmvaeffcm,*cmvaeffcl;
  Weights *cmvaeffot,*cmvaeffom,*cmvaeffol;

  class BTagWeight
  {
  private:
    int minTags;
    int maxTags;
  public:
    struct JetInfo
    {
      JetInfo(float mceff, float datasf) : eff(mceff), sf(datasf) {}
      float eff;
      float sf;
    };
    BTagWeight():
      minTags(0), maxTags(0)
    {
      ;
    }
    BTagWeight(int jmin, int jmax) :
      minTags(jmin) , maxTags(jmax) {}
    bool filter(int t);
    float weight(vector<JetInfo> jets, int tags);
    float weightWithVeto(vector<JetInfo> jetsTags, int tags, vector<JetInfo> jetsVetoes, int vetoes);
  };  

  double MCTagEfficiency(string algo, int flavor, double pt, double eta); 
  double TagScaleFactor(string algo, int flavor, string syst,double pt, double eta); 

  std::vector<BTagWeight::JetInfo> jsfscmvat, 
    jsfscmvat_b_tag_up, 
    jsfscmvat_b_tag_down, 
    jsfscmvat_mistag_up, 
    jsfscmvat_mistag_down;

  std::vector<BTagWeight::JetInfo> jsfscmvam, 
    jsfscmvam_b_tag_up, 
    jsfscmvam_b_tag_down, 
    jsfscmvam_mistag_up, 
    jsfscmvam_mistag_down;

  std::vector<BTagWeight::JetInfo> jsfscmval, 
    jsfscmval_b_tag_up, 
    jsfscmval_b_tag_down, 
    jsfscmval_mistag_up, 
    jsfscmval_mistag_down;
  
  BTagWeight b_cmvat_0_tags= BTagWeight(0,0),
    b_cmvat_1_tag= BTagWeight(1,1),
    b_cmvat_2_tags= BTagWeight(2,2);
  
  double b_weight_cmvat_0_tags,
    b_weight_cmvat_1_tag,
    b_weight_cmvat_2_tags;
  double b_weight_cmvat_0_tags_mistag_up,
    b_weight_cmvat_1_tag_mistag_up,
    b_weight_cmvat_2_tags_mistag_up;
  double b_weight_cmvat_0_tags_mistag_down,
    b_weight_cmvat_1_tag_mistag_down,
    b_weight_cmvat_2_tags_mistag_down;
  double b_weight_cmvat_0_tags_b_tag_down,
    b_weight_cmvat_1_tag_b_tag_down,
    b_weight_cmvat_2_tags_b_tag_down;
  double b_weight_cmvat_0_tags_b_tag_up,
    b_weight_cmvat_1_tag_b_tag_up,
    b_weight_cmvat_2_tags_b_tag_up;

  BTagWeight b_cmvam_0_tags= BTagWeight(0,0),
    b_cmvam_1_tag= BTagWeight(1,1),
    b_cmvam_2_tags= BTagWeight(2,2);

  double b_weight_cmvam_0_tags,
    b_weight_cmvam_1_tag,
    b_weight_cmvam_2_tags;
  double b_weight_cmvam_0_tags_mistag_up,
    b_weight_cmvam_1_tag_mistag_up,
    b_weight_cmvam_2_tags_mistag_up;
  double b_weight_cmvam_0_tags_mistag_down,
    b_weight_cmvam_1_tag_mistag_down,
    b_weight_cmvam_2_tags_mistag_down;
  double b_weight_cmvam_0_tags_b_tag_down,
    b_weight_cmvam_1_tag_b_tag_down,
    b_weight_cmvam_2_tags_b_tag_down;
  double b_weight_cmvam_0_tags_b_tag_up,
    b_weight_cmvam_1_tag_b_tag_up,
    b_weight_cmvam_2_tags_b_tag_up;

  BTagWeight b_cmval_0_tags= BTagWeight(0,0),
    b_cmval_1_tag= BTagWeight(1,1),
    b_cmval_2_tags= BTagWeight(2,2);
  
  double b_weight_cmval_0_tags_mistag_up,
    b_weight_cmval_1_tag_mistag_up,
    b_weight_cmval_2_tags_mistag_up;
  double b_weight_cmval_0_tags_mistag_down,
    b_weight_cmval_1_tag_mistag_down,
    b_weight_cmval_2_tags_mistag_down;
  double b_weight_cmval_0_tags_b_tag_down,
    b_weight_cmval_1_tag_b_tag_down,
    b_weight_cmval_2_tags_b_tag_down;
  double b_weight_cmval_0_tags_b_tag_up,
    b_weight_cmval_1_tag_b_tag_up,
    b_weight_cmval_2_tags_b_tag_up;
  double b_weight_cmval_0_tags,
    b_weight_cmval_1_tag,
    b_weight_cmval_2_tags;
  
    bool doBTagSF=true;
    bool doTopPtReweight;

};


DMAnalysisTreeMaker::DMAnalysisTreeMaker(const edm::ParameterSet& iConfig){
  
  mu_label = iConfig.getParameter<std::string >("muLabel");
  ele_label = iConfig.getParameter<std::string >("eleLabel");
  jets_label = iConfig.getParameter<std::string >("jetsLabel");
  met_label = iConfig.getParameter<std::string >("metLabel");
  metnohf_label = iConfig.getParameter<std::string >("metnohfLabel");
  jetsnohf_label = iConfig.getParameter<std::string >("jetsnohfLabel");
  physObjects = iConfig.template getParameter<std::vector<edm::ParameterSet> >("physicsObjects");
  
  channelInfo = iConfig.getParameter<edm::ParameterSet >("channelInfo"); // The physics of the channel, e.g. the cross section, #original events, etc.
  channel = channelInfo.getParameter<std::string>("channel");
  crossSection = channelInfo.getParameter<double>("crossSection");
  originalEvents = channelInfo.getParameter<double>("originalEvents");
  useLHEWeights = channelInfo.getUntrackedParameter<bool>("useLHEWeights",false);
  //useLHE = channelInfo.getUntrackedParameter<bool>("useLHE",false);
  useLHE = channelInfo.getUntrackedParameter<bool>("useLHE",false);
  doTopPtReweight= channelInfo.getUntrackedParameter<bool>("topPtreweight",false);	
  //addLHAPDFWeights = channelInfo.getUntrackedParameter<bool>("addLHAPDFWeights",false);
  addLHAPDFWeights = channelInfo.getUntrackedParameter<bool>("addLHAPDFWeights",true);

  if( useLHE ){
    edm::InputTag lhes_ = iConfig.getParameter<edm::InputTag>( "lhes" );
    t_lhes_ = consumes< LHEEventProduct >( lhes_ );
  }

  if(doTopPtReweight){
	t_genParticles_ = consumes <std::vector<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>( "filteredPrunedGenParticles" ));
  }
		
  if( addLHAPDFWeights ){
    edm::InputTag genprod_ = iConfig.getParameter<edm::InputTag>( "genprod" );
    t_genprod_ = consumes<GenEventInfoProduct>( genprod_ );
  }

  useTriggers = iConfig.getUntrackedParameter<bool>("useTriggers",true);
  cutOnTriggers = iConfig.getUntrackedParameter<bool>("cutOnTriggers",true);
	
  edm::InputTag lumiBlock_ = iConfig.getParameter<edm::InputTag>("lumiBlock");
  t_lumiBlock_ = consumes< unsigned int >( lumiBlock_ );
  edm::InputTag runNumber_ = iConfig.getParameter<edm::InputTag>("runNumber");
  t_runNumber_ = consumes< unsigned int >( runNumber_ );
  edm::InputTag eventNumber_ = iConfig.getParameter<edm::InputTag>("eventNumber");
  t_eventNumber_ = consumes< ULong64_t >( eventNumber_ );

  if(useTriggers){
    edm::InputTag triggerBits_ = iConfig.getParameter<edm::InputTag>("triggerBits");
    t_triggerBits_ = consumes< std::vector<float> >( triggerBits_ );
    edm::InputTag triggerNames_ = iConfig.getParameter<edm::InputTag>("triggerNames");
    t_triggerNames_ = consumes< std::vector<string>, edm::InRun>( triggerNames_ );
    edm::InputTag triggerPrescales_ = iConfig.getParameter<edm::InputTag>("triggerPrescales");
    t_triggerPrescales_ = consumes< std::vector<int> >( triggerPrescales_ );
    leptonicTriggers= channelInfo.getParameter<std::vector<string> >("leptonicTriggers");
    hadronicTriggers= channelInfo.getParameter<std::vector<string> >("hadronicTriggers");
  }

  useMETFilters = iConfig.getUntrackedParameter<bool>("useMETFilters",true);
  if(useMETFilters){
    metFilters = channelInfo.getParameter<std::vector<string> >("metFilters");
    metBits_ = iConfig.getParameter<edm::InputTag>("metBits");
    t_metBits_ = consumes< std::vector<float> >( metBits_ );
    metNames_ = iConfig.getParameter<edm::InputTag>("metNames");
	t_metNames_ = consumes< std::vector<string>, edm::InRun >( metNames_ );    

//  t_HBHEFilter_ = consumes< bool >(iConfig.getParameter<edm::InputTag>("HBHEFilter") );
//	t_HBHEIsoFilter_= consumes< bool >(iConfig.getParameter<edm::InputTag>("HBHEIsoFilter"));

  }

  addPV = iConfig.getUntrackedParameter<bool>("addPV",true);
  changeJECs = iConfig.getUntrackedParameter<bool>("changeJECs",false);
  isData = iConfig.getUntrackedParameter<bool>("isData",false);
  if(isData) Era=iConfig.getParameter<std::string>("era");	

  useMETNoHF = iConfig.getUntrackedParameter<bool>("useMETNoHF",false);

  if( changeJECs )
    t_Rho_ = consumes<double>( edm::InputTag( "fixedGridRhoFastjetAll" ) ) ;

  if(addPV || changeJECs){
    edm::InputTag pvZ_ = iConfig.getParameter<edm::InputTag >("vertexZ");
    t_pvZ_ = consumes< std::vector<float> >( pvZ_ );
    edm::InputTag pvChi2_ = iConfig.getParameter<edm::InputTag >("vertexChi2");
    t_pvChi2_ = consumes< std::vector<float> >( pvChi2_ );
    edm::InputTag pvRho_ = iConfig.getParameter<edm::InputTag >("vertexRho");
    t_pvRho_ = consumes< std::vector<float> >( pvRho_ );
    edm::InputTag pvNdof_ = iConfig.getParameter<edm::InputTag >("vertexNdof");
    t_pvNdof_ = consumes< std::vector< int > >( pvNdof_ );
  }

  //---------------- Soureek adding PU info -----------------------------------------
  doPU_=iConfig.getParameter<bool>("doPU");
  dataPUFile_=iConfig.getParameter<std::string>("dataPUFile");
  if( !isData && doPU_ ){
//	dataPUFile_=iConfig.getParameter<std::string>("dataPUFile");
    t_ntrpu_ = consumes< int >( edm::InputTag( "eventUserData","puNtrueInt" ) );
  //---------------------------------------------------------------------------------
  }	
  
  if(useLHEWeights){
    maxWeights = channelInfo.getUntrackedParameter<int>("maxWeights",9);//Usually we do have 9 weights for the scales, might vary depending on the lhe
  }

  
  if(addLHAPDFWeights){
    centralPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","CT10");
    //centralPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","NNPDF");
    variationPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","CT10");
    //variationPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","NNPDF");
    initializePdf(centralPdfSet,variationPdfSet);

  }
  

  systematics = iConfig.getParameter<std::vector<std::string> >("systematics");

  jetScanCuts = iConfig.getParameter<std::vector<double> >("jetScanCuts");
  
  
  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();

  bool addNominal=false;
  for (size_t s = 0; s<systematics.size();++s){
    if(systematics.at(s).find("noSyst")!=std::string::npos) {
      addNominal=true;
      break;
    }
  }
  if(systematics.size()==0){
    addNominal=true;
    systematics.push_back("noSyst");
  }//In case there's no syst specified, do the nominal scenario
  //addNominal=true;
  Service<TFileService> fs;
  TFileDirectory DMTrees;// = fs->mkdir( "systematics_trees" );


  if(addNominal){
    //Service<TFileService> fs;
    DMTrees = fs->mkdir( "systematics_trees" );
  }
  //  treesBase = new TTree("TreeBase", "TreeBase");
  trees["noSyst"] =  new TTree((channel+"__noSyst").c_str(),(channel+"__noSyst").c_str());
  
  
  for (;itPsets!=physObjects.end();++itPsets){ 
    int maxI = itPsets->getUntrackedParameter< int >("maxInstances",10);
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    string namelabel = itPsets->getParameter< string >("label");
    string nameprefix = itPsets->getParameter< string >("prefix");
    bool saveBaseVariables = itPsets->getUntrackedParameter<bool>("saveBaseVariables",true);
    std::vector<std::string > toSave= itPsets->getParameter<std::vector<std::string> >("toSave");
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();

    stringstream max_instance_str;
    max_instance_str<<maxI;
    max_instances[namelabel]=maxI;
    string nameobs = namelabel;
    string prefix = nameprefix;
  
	trees["noSyst"]->Branch((nameobs+"_size").c_str(), &sizes[nameobs]);
  
	for (;itF != variablesFloat.end();++itF){
      
      string name=itF->instance()+"_"+itF->label();
      string nameinstance=itF->instance();
      string nameshort=itF->instance();
      //if(nameobs!=itF->label())cout<<" warning! label not matching the module! check if members of pset are consistent. If intentional , ignore this warning";
      //cout << "nameobs "<< nameobs<< " name " << name <<" nameshort " <<nameshort << " strsizw "<< (nameshort+"["+max_instance_str.str()+"]/F").c_str()<<endl;
      //      std::cout << " label is " << itF->instance()<< " is in vector? "<< isInVector(toSave,itF->instance())<< endl;
      string nametobranch = makeBranchName(namelabel,prefix,nameinstance);
      name = nametobranch;
      nameshort = nametobranch;
      //      cout << " name for the branch is now: "<< nametobranch<<endl;
      //      if(saveBaseVariables|| isInVector(toSave,itF->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+max_instance_str.str()+"]/F").c_str());
      if(saveBaseVariables|| isInVector(toSave,itF->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+nameobs+"_size"+"]/F").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;

      t_floats[ name ] = consumes< std::vector<float> >( *itF );

      cout << " branching name "<< name<< " for obs "<< nameobs << " instance "<< nameinstance << endl;
    }
    
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+"_"+itI->label();
      string nameshort=itF->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      if(saveBaseVariables|| isInVector(toSave,itI->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vints_values[name],(nameshort+"["+nameobs+"_size"+"]/I").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;


      t_ints[ name ] = consumes< std::vector<int> >( *itI );
    }  
//    std::cout<<"Check point Constructor 1"<<std::endl;
     
    if (variablesFloat.size()>0){
      string nameshortv = namelabel;
      vector<string> extravars = additionalVariables(nameshortv);
      for(size_t addv = 0; addv < extravars.size();++addv){
	string name = nameshortv+"_"+extravars.at(addv);
	if (saveBaseVariables || isInVector(toSave, extravars.at(addv)) || isInVector(toSave, "allExtra") )trees["noSyst"]->Branch(name.c_str(), &vfloats_values[name],(name+"["+nameobs+"_size"+"]/F").c_str());
	obs_to_obj[name] = nameobs;
	obj_to_pref[nameobs] = prefix;
      }
    }
    names.push_back(nameobs);
    cout << "size part: nameobs is  "<< nameobs<<endl;
    trees["noSyst"]->Branch((nameobs+"_size").c_str(), &sizes[nameobs]);
    
    //Initialize single pset objects
    for (;itsF != singleFloat.end();++itsF){
      string name=itsF->instance()+itsF->label();
      string nameshort=itsF->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      t_float[ name ] = consumes< float >( *itsF );
      if(saveBaseVariables|| isInVector(toSave,itsF->instance()))trees["noSyst"]->Branch(nameshort.c_str(), &float_values[name]);
    }

    for (;itsI != singleInt.end();++itsI){
      string name=itsI->instance()+itsI->label();
      string nameshort=itsI->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      t_int[ name ] = consumes< int >( *itsI );
      if(saveBaseVariables|| isInVector(toSave,itsI->instance()))trees["noSyst"]->Branch(nameshort.c_str(), &int_values[name]);
    }
  }
  
//  std::cout<<"Check point Constructor 2"<<std::endl;

  string nameshortv= "Event";
  std::vector<std::string> extravars = additionalVariables(nameshortv);
  for(size_t addv = 0; addv < extravars.size();++addv){
    string name = nameshortv+"_"+extravars.at(addv);
    if (name.find("EventNumber") != std::string::npos)
      trees["noSyst"]->Branch(name.c_str(), &int_values[name],(name+"/I").c_str());
    else
      trees["noSyst"]->Branch(name.c_str(), &float_values[name],(name+"/F").c_str());
  }
//  std::cout<<"Check point Constructor 3"<<std::endl;
  
  //Prepare the trees cloning all branches and setting the correct names/titles:
  if(!addNominal){
    DMTrees = fs->mkdir( "systematics_trees" );
  }
  	
  for(size_t s=0;s< systematics.size();++s){
    std::string syst  = systematics.at(s);
//    if(syst=="noSyst")continue;
    if(syst!="noSyst"){ 
		trees[syst]= trees["noSyst"]->CloneTree();
		//trees[syst]= treesBase->CloneTree();
		trees[syst]->SetName((channel+"__"+syst).c_str());
		trees[syst]->SetTitle((channel+"__"+syst).c_str());
	}
	
    if(TString(syst).Contains(TString("jes"))){
		jes_syst="";
		for(int i=4;i<(int)syst.size();i++){
			singleNumeral=syst.at(i);
			if(singleNumeral == '_'){ 
				if(i>4){ 
					jNumeral=i; 
					std::cout<<syst<<"\t"<<jNumeral<<std::endl;
					break;
				}		
			}
		}
	
	
		jes_syst.append(syst,4,jNumeral-4);	
		std::cout<<jes_syst<<std::endl;

		if(isData) {

			if(Era=="RunB" || Era=="RunC" || Era=="RunD"){
				jecParsL1  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt");
				jecParsL2  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2Residual_AK4PFchs.txt");
				jecParsL3  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt");
				jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt");     
				jecUnc[syst]  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_UncertaintySources_AK4PFchs.txt", jes_syst.c_str())));
			}

			if(Era=="RunE" || Era=="RunF"){
				jecParsL1  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt");
				jecParsL2  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2Residual_AK4PFchs.txt");
				jecParsL3  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt");
				jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt");     
				jecUnc[syst]  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016EFV4_DATA_UncertaintySources_AK4PFchs.txt", jes_syst.c_str())));
			}

			if(Era=="RunG"){
				jecParsL1  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt");
				jecParsL2  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016GV4_DATA_L2Residual_AK4PFchs.txt");
				jecParsL3  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt");
				jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt");     
				jecUnc[syst]  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016GV4_DATA_UncertaintySources_AK4PFchs.txt", jes_syst.c_str())));
			}

			if(Era=="RunH"){	
				jecParsL1  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt");
				jecParsL2  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt");
				jecParsL3  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt");
				jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt");		
				jecUnc[syst]  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt", jes_syst.c_str())));
			}

		}

		else {
			jecParsL1 = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt");
			jecParsL2  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt");
			jecParsL3  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt");
			jecUnc[syst]  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt", jes_syst.c_str())));
		}

		jecPars.push_back(*jecParsL1);
		jecPars.push_back(*jecParsL2);
		jecPars.push_back(*jecParsL3);

		if(isData) jecPars.push_back(*jecParsL2L3Residuals);

		jecCorr = new FactorizedJetCorrector(jecPars);	
	  }		
	}	


/*  if(isData) {

	if(Era=="RunB" || Era=="RunC" || Era=="RunD"){
        jecParsL1  = new JetCorrectorParameters("Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt");
        jecParsL2  = new JetCorrectorParameters("Summer16_23Sep2016BCDV4_DATA_L2Residual_AK4PFchs.txt");
        jecParsL3  = new JetCorrectorParameters("Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt");
        jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt");     
        jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16_23Sep2016BCDV4_DATA_UncertaintySources_AK4PFchs.txt", "Total")));
	}

	if(Era=="RunE" || Era=="RunF"){
        jecParsL1  = new JetCorrectorParameters("Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt");
        jecParsL2  = new JetCorrectorParameters("Summer16_23Sep2016EFV4_DATA_L2Residual_AK4PFchs.txt");
        jecParsL3  = new JetCorrectorParameters("Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt");
        jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt");     
        jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16_23Sep2016EFV4_DATA_UncertaintySources_AK4PFchs.txt", "Total")));

	}

	if(Era=="RunG"){
        jecParsL1  = new JetCorrectorParameters("Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt");
        jecParsL2  = new JetCorrectorParameters("Summer16_23Sep2016GV4_DATA_L2Residual_AK4PFchs.txt");
        jecParsL3  = new JetCorrectorParameters("Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt");
        jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt");     
        jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16_23Sep2016GV4_DATA_UncertaintySources_AK4PFchs.txt", "Total")));

	}

	if(Era=="RunH"){	
		jecParsL1  = new JetCorrectorParameters("Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt");
		jecParsL2  = new JetCorrectorParameters("Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt");
		jecParsL3  = new JetCorrectorParameters("Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt");
		jecParsL2L3Residuals  = new JetCorrectorParameters("Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt");		
		jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt", "Total")));
	}
  }

  else {
    jecParsL1  = new JetCorrectorParameters("Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt");
    jecParsL2  = new JetCorrectorParameters("Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt");
    jecParsL3  = new JetCorrectorParameters("Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt");
	jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt", "Total")));
  }

  jecPars.push_back(*jecParsL1);
  jecPars.push_back(*jecParsL2);
  jecPars.push_back(*jecParsL3);

  if(isData) jecPars.push_back(*jecParsL2L3Residuals);

  jecCorr = new FactorizedJetCorrector(jecPars);
*/ 
  
//  jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("", "Total")));
	
  filename_cmva="btagging_cmva.root";
  file_cmva= TFile::Open(filename_cmva);
  cmvaeffbt = new Weights(file_cmva,"b__tight");
  cmvaeffbl = new Weights(file_cmva,"b__loose");
  cmvaeffbm = new Weights(file_cmva,"b__medium");

  cmvaeffct = new Weights(file_cmva,"c__tight");
  cmvaeffcl = new Weights(file_cmva,"c__medium");
  cmvaeffcm = new Weights(file_cmva,"c__loose");

  cmvaeffot = new Weights(file_cmva,"other__tight");
  cmvaeffol = new Weights(file_cmva,"other__medium");
  cmvaeffom = new Weights(file_cmva,"other__loose");

  calib_cmvav2 = new BTagCalibration("CMVAv2","cMVAv2_Moriond17_B_H.csv");
  readerCMVALoose = new BTagCalibrationReader(BTagEntry::OP_LOOSE, "central", {"up", "down"});      
  readerCMVALoose->load(*calib_cmvav2, BTagEntry::FLAV_B,   "ttbar");
  readerCMVALoose->load(*calib_cmvav2, BTagEntry::FLAV_C,   "ttbar");
  readerCMVALoose->load(*calib_cmvav2, BTagEntry::FLAV_UDSG,   "incl");

  readerCMVAMedium = new BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up", "down"});      
  readerCMVAMedium->load(*calib_cmvav2, BTagEntry::FLAV_B,   "ttbar");
  readerCMVAMedium->load(*calib_cmvav2, BTagEntry::FLAV_C,   "ttbar");
  readerCMVAMedium->load(*calib_cmvav2, BTagEntry::FLAV_UDSG,   "incl");

  readerCMVATight = new BTagCalibrationReader(BTagEntry::OP_TIGHT, "central", {"up", "down"});      
  readerCMVATight->load(*calib_cmvav2, BTagEntry::FLAV_B,   "ttbar");
  readerCMVATight->load(*calib_cmvav2, BTagEntry::FLAV_C,   "ttbar");
  readerCMVATight->load(*calib_cmvav2, BTagEntry::FLAV_UDSG,   "incl");

//  readerCMVAReshape = new BTagCalibrationReader(BTagEntry::OP_RESHAPING, "central", {"up_jes", "down_jes"});      
//  readerCMVAReshape->load(*calib_cmvav2, BTagEntry::FLAV_B,   "iterativefit");
//  readerCMVAReshape->load(*calib_cmvav2, BTagEntry::FLAV_C,   "iterativefit");
//  readerCMVAReshape->load(*calib_cmvav2, BTagEntry::FLAV_UDSG, "iterativefit");  

//  if(addNominal) systematics.push_back("noSyst");

}

void DMAnalysisTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();

  //  if(addLHAPDFWeights){

  // event info
  iEvent.getByToken(t_lumiBlock_,lumiBlock );
  iEvent.getByToken(t_runNumber_,runNumber );
  iEvent.getByToken(t_eventNumber_,eventNumber );

  if(useLHE){
    iEvent.getByToken(t_lhes_, lhes);
  }
  if(addLHAPDFWeights){
    iEvent.getByToken(t_genprod_, genprod);
  }

  if(doTopPtReweight){
//    edm::Handle<std::vector<reco::GenParticle>> prunedGenParticles;
    iEvent.getByToken(t_genParticles_,prunedGenParticles);
  }  	
  //Part 0: trigger preselection
  if(useTriggers){
    iEvent.getByToken(t_triggerBits_,triggerBits );
//    iEvent.getByToken(t_triggerNames_,triggerNames );
    iEvent.getByToken(t_triggerPrescales_,triggerPrescales );
    bool triggerOr = getEventTriggers();
    if(cutOnTriggers && !triggerOr) return;
  }  


  if(useMETFilters){
    iEvent.getByToken(t_metBits_,metBits );
//    iEvent.getByToken(t_metNames_,metNames );
//    iEvent.getByToken(t_HBHEFilter_ ,HBHE);
//    iEvent.getByToken(t_HBHEIsoFilter_ ,HBHEIso);

	if(isFirstEvent){
//		for(size_t bt = 0; bt < metNames->size();++bt){
//			std::string tname = metNames->at(bt);
//			std::cout << "test tname "<< tname << " passes "<< metBits->at(bt)<< endl;
//		}

    	getMETFilters();
  	}
  }	
  
//  std::cout<<"Check 1"<<std::endl;
  
  if(changeJECs){
    iEvent.getByToken(t_Rho_ ,rho);
    Rho = *rho; 
  }
  if( addPV || changeJECs){
    iEvent.getByToken(t_pvZ_,pvZ);
    iEvent.getByToken(t_pvChi2_,pvChi2);
    iEvent.getByToken(t_pvNdof_,pvNdof);
    iEvent.getByToken(t_pvRho_,pvRho);
    nPV = pvZ->size();
  }

  if(isFirstEvent) isFirstEvent=false;		

  /*std::cout << " initTriggers "<< endl;
    for(size_t bt = 0; bt < triggerNames->size();++bt){
    std::string tname = triggerNames->at(bt);
    float bit = triggerBits->at(bt);
    int presc = triggerPrescales->at(bt);
    std::cout << "name "<< tname << " bit "<< bit << " prescale "<<presc<<endl;
    }*/
    
  
    
//  std::cout<<"Collected # of primary vertices: "<<nPV<<std::endl;	  

  //Part 1 taking the obs values from the edm file
  for (;itPsets!=physObjects.end();++itPsets){ 
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();
    string namelabel = itPsets->getParameter< string >("label");
    string nameprefix = itPsets->getParameter< string >("prefix");
    size_t maxInstance=(size_t)max_instances[namelabel];

    //Vectors of floats
    for (;itF != variablesFloat.end();++itF){
      string varname=itF->instance();
      
      string name = makeBranchName(namelabel,nameprefix,varname);
      
      //string namelabel;
      float tmp =1.0;
      iEvent.getByToken(t_floats[name] ,h_floats[name]);
      //      cout << "name "<< name <<endl;
      for (size_t fi = 0;fi < maxInstance ;++fi){
	if(fi <h_floats[name]->size()){tmp = h_floats[name]->at(fi);}
	else { tmp = -9999.; }
	//	cout << " setting name "<< name<< " at instance "<< fi <<" to value "<< tmp <<endl;
	vfloats_values[name][fi]=tmp;
      }
      sizes[namelabel]=h_floats[name]->size();
      //      cout<< " size for "<< namelabel <<" is then "<< sizes[namelabel]<<endl; 
    }
    
//       std::cout << " checkpoint floats"<<endl;
    //Vectors of ints
    for (;itI != variablesInt.end();++itI){
      string varname=itI->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);
      int tmp = 1;
      iEvent.getByToken(t_ints[name] ,h_ints[name]);
      for (size_t fi = 0;fi < maxInstance;++fi){
	if(fi <h_ints[name]->size()){tmp = h_ints[name]->at(fi);}
	else { tmp = -9999.; }
	vints_values[name][fi]=tmp;
      }
    }  
    
//        std::cout << " checkpoint ints"<<endl;
    //Single floats/ints
    for (;itsF != singleFloat.end();++itsF){
      string varname=itsF->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);

      iEvent.getByToken(t_float[name],h_float[name]);
      float_values[name]=*h_float[name];
    }
    for (;itsI != singleInt.end();++itsI){
      string varname=itsI->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);
      iEvent.getByToken(t_int[name],h_int[name]);
      int_values[name]=*h_int[name];
    }
//       std::cout << " checkpoint singles"<<endl;
  }

//    std::cout << " checkpoint part 1"<<endl;


  //Part 2: selection and analysis-level changes
  //This might change for each particular systematics, 
  //e.g. for each jet energy scale variation, for MET or even lepton energy scale variations
  vector<TLorentzVector> electrons;
  vector<TLorentzVector> muons;
  vector<TLorentzVector> leptons;
  vector<TLorentzVector> loosemuons;
  vector<TLorentzVector> jets;
  vector<TLorentzVector> muons_t;
  vector<TLorentzVector> muons_t_2p4;

  for (size_t s = 0; s< systematics.size();++s){

//    int ncsvl_tags=0,ncsvt_tags=0,ncsvm_tags=0;//, njets_tottag=0;    
    int ncmval_tags=0,ncmvat_tags=0,ncmvam_tags=0;//, njets_tottag=0;    

    if(!useTriggers) getEventTriggers();

    electrons.clear();
    muons.clear();
    loosemuons.clear();
    jets.clear();
    string syst = systematics.at(s);

    jsfscmvat.clear();
    jsfscmvat_b_tag_up.clear(); 
    jsfscmvat_b_tag_down.clear(); 
    jsfscmvat_mistag_up.clear(); 
    jsfscmvat_mistag_down.clear();

    jsfscmvam.clear(); 
    jsfscmvam_b_tag_up.clear(); 
    jsfscmvam_b_tag_down.clear(); 
    jsfscmvam_mistag_up.clear(); 
    jsfscmvam_mistag_down.clear();
    
    jsfscmval.clear(); 
    jsfscmval_b_tag_up.clear(); 
    jsfscmval_b_tag_down.clear(); 
    jsfscmval_mistag_up.clear();
    jsfscmval_mistag_down.clear();

  //---------------- Soureek Adding PU Info ------------------------------
    if(doPU_){
      iEvent.getByToken(t_ntrpu_,ntrpu);
      nTruePU=*ntrpu;
      getPUSF();
    }

//    std::cout<<"Check for PU re-weighting 2"<<std::endl;
    /**************************
    Muons:
    **************************/
    float mu_sf = 1;
    float mu_sf_up = 1;
    float mu_sf_down = 1;

    for(int mu = 0;mu < max_instances[mu_label] ;++mu){
      string pref = obj_to_pref[mu_label];
      //      std::cout << " now checking name " << makeName(mu_label,pref,"IsTightMuon")<<std::endl;
      float isTight = vfloats_values[makeName(mu_label,pref,"IsTightMuon")][mu];
      float isLoose = vfloats_values[makeName(mu_label,pref,"IsLooseMuon")][mu];
      float isSoft = vfloats_values[makeName(mu_label,pref,"IsSoftMuon")][mu];
      //      float isVeto = vfloats_values[makeName(ele_label,pref,"isVeto")][el];
      float pt = vfloats_values[makeName(mu_label,pref,"Pt")][mu];
      float eta = vfloats_values[makeName(mu_label,pref,"Eta")][mu];
      float phi = vfloats_values[makeName(mu_label,pref,"Phi")][mu];
      float energy = vfloats_values[makeName(mu_label,pref,"E")][mu];
      float iso = vfloats_values[makeName(mu_label,pref,"Iso04")][mu];
      //      std::cout << " muon #"<<mu<< " pt " << pt << " isTight/Loose/Soft?"<< isTight<<isSoft<<isLoose<<std::endl;
      
      if(isTight>0 && pt> 24 && abs(eta) < 2.4 && iso <0.06){
	++float_values["Event_nTightMuons"];
	TLorentzVector muon;
	muon.SetPtEtaPhiE(pt, eta, phi, energy);
	muons.push_back(muon);
	muons_t.push_back(muon);

	if (abs(eta) < 2.4)
	  muons_t_2p4.push_back(muon);

	leptons.push_back(muon);

	mu_sf *= muonSF(isData,pt,eta,0);
	mu_sf_up *= muonSF(isData,pt,eta,1);
	mu_sf_down *= muonSF(isData,pt,eta,-1);

      }
      if(isLoose>0 && pt> 10 && abs(eta) < 2.1 && iso<0.2){
	++float_values["Event_nLooseMuons"];
      }
      if(isSoft>0 && pt> 30 && abs(eta) < 2.4){
	++float_values["Event_nSoftMuons"]; 
      }
      if(isLoose>0 && pt > 15){
	TLorentzVector muon;
	muon.SetPtEtaPhiE(pt, eta, phi, energy);
	loosemuons.push_back(muon);
      }
    }

	if(!isData){
		float_values["Event_mu_eff"]=mu_sf;
		float_values["Event_mu_eff_up"]=mu_sf_up;
		float_values["Event_mu_eff_down"]=mu_sf_down;
	}
//	std::cout<<"Ending Muons"<<std::endl;
	
    /**************************
    Electrons:
    *const edm::ParameterSet&*************************/
    for(int el = 0;el < max_instances[ele_label] ;++el){
      string pref = obj_to_pref[ele_label];
      float pt = vfloats_values[makeName(ele_label,pref,"Pt")][el];
      //      if(pt<0)continue;

      float eta = vfloats_values[makeName(ele_label,pref,"Eta")][el];
      float scEta = vfloats_values[makeName(ele_label,pref,"SCEta")][el];
      float phi = vfloats_values[makeName(ele_label,pref,"Phi")][el];
      float energy = vfloats_values[makeName(ele_label,pref,"E")][el];      
      float iso = vfloats_values[makeName(ele_label,pref,"Iso03")][el];
	  float eldxy = vfloats_values[makeName(ele_label,pref,"Dxy")][el];
	  float eldz =  vfloats_values[makeName(ele_label,pref,"Dz")][el];
	  float elCharge = vfloats_values[makeName(ele_label,pref,"Charge")][el];	


      float isTight = vfloats_values[makeName(ele_label,pref,"vidTight")][el];
      float isLoose = vfloats_values[makeName(ele_label,pref,"vidLoose")][el];
      float isMedium = vfloats_values[makeName(ele_label,pref,"vidMedium")][el];
      float isVeto = vfloats_values[makeName(ele_label,pref,"vidVeto")][el];
      float vidHEEP = vfloats_values[makeName(ele_label,pref,"vidHEEP")][el];
       
      
      //cout << "######" << endl;
      //std::cout << " electron #"<<el<< " pt " << pt << " isTight/Mid/Loose/Veto?"<< isTight<<isMedium<<isLoose<<isVeto<<std::endl;
      //std::cout << " electron #"<<el<< " pt " << pt << " vid isTight/Mid/Loose/Veto?"<< vidTight<<vidMedium<<vidLoose<<vidVeto<<vidHEEP<<std::endl;

      bool passesDRmu = true;
      bool passesTightCuts = false;
      if(fabs(scEta)<=1.479){
		passesTightCuts = ( isTight >0.0 /*&& iso < 0.0588 */) && (fabs(eldz) < 0.10) && (fabs(eldxy) <0.05 ) ;
      } //is barrel electron

	  if( ( fabs(scEta)>1.479 && fabs(scEta)<2.5 ) && ( (fabs(eldz) < 0.20) && (fabs(eldxy) < 0.10) ) ){
		passesTightCuts = isTight >0.0 /*&& iso < 0.0571*/ ;
      }

    if(pt> 30 && abs(eta) < 2.1 && passesTightCuts){
	TLorentzVector ele;
	ele.SetPtEtaPhiE(pt, eta, phi, energy);	
	double minDR=999;
	double deltaR = 999;
	for (size_t m = 0; m < (size_t)loosemuons.size(); ++m){
	  deltaR = ele.DeltaR(loosemuons[m]);
	  minDR = min(minDR, deltaR);
	}
	if(!loosemuons.size()) minDR=999;
	if(minDR>0.1){ 
	  electrons.push_back(ele); 
	  leptons.push_back(ele);
	  ++float_values["Event_nTightElectrons"];

	}
	else {passesDRmu = false;}
      }

      if(isLoose>0 && pt> 30 && abs(eta) < 2.5){
	++float_values["Event_nLooseElectrons"];
      }
      if(isMedium>0 && pt> 30 && abs(eta) < 2.5){
	++float_values["Event_nMediumElectrons"]; 
      }

      if(isVeto>0 && pt> 15 && abs(eta) < 2.5){

		if((fabs(scEta)<=1.479 && (fabs(eldz) < 0.10) && (fabs(eldxy) <0.05 )) || ((fabs(scEta)>1.479 && fabs(scEta)<2.5) && (fabs(eldz) < 0.20) && (fabs(eldxy) < 0.10) )){
			++float_values["Event_nVetoElectrons"]; 
		}
	  }	

      vfloats_values[ele_label+"_PassesDRmu"][el]=(float)passesDRmu;

    }
     

//	std::cout<<"Ending Electrons"<<std::endl;
    /**************************
    MET:
    **************************/
    //    cout << " met "<<endl;

    string pref = obj_to_pref[met_label];
    float metpt = vfloats_values[makeName(met_label,pref,"Pt")][0];
    float metphi = vfloats_values[makeName(met_label,pref,"Phi")][0];
    float metZeroCorrPt = vfloats_values[makeName(met_label,pref,"uncorrPt")][0];
    float metZeroCorrPhi = vfloats_values[makeName(met_label,pref,"uncorrPhi")][0];
    float metZeroCorrY = metZeroCorrPt*sin(metZeroCorrPhi);
    float metZeroCorrX = metZeroCorrPt*cos(metZeroCorrPhi);
    float metPx = metpt*sin(metphi);
    float metPy = metpt*cos(metphi);
    
//   std::cout<<"Ending MET"<<std::endl;
  
    //    cout << "syst "<<syst<<endl;
    /**************************
    Jets:
    **************************/
    double corrMetPx =0;
    double corrMetPy =0;
    double DUnclusteredMETPx=0.0;
    double DUnclusteredMETPy=0.0;

    for(int j = 0;j < max_instances[jets_label] ;++j){

      string pref = obj_to_pref[jets_label];
      float pt = vfloats_values[makeName(jets_label,pref,"Pt")][j];
      //      if(pt<0)continue;
      float genpt = vfloats_values[makeName(jets_label,pref,"GenJetPt")][j];
      float eta = vfloats_values[makeName(jets_label,pref,"Eta")][j];
      float phi = vfloats_values[makeName(jets_label,pref,"Phi")][j];
      float energy = vfloats_values[makeName(jets_label,pref,"E")][j];
      float ptCorr = -9999;
      float energyCorr = -9999;
      float smearfact = -9999;
//      std::cout << "j is " <<j<< "label "<< jets_label << " maxinstances "<< max_instances[jets_label]<< "size "<< sizes[jets_label]<< " pt "<< vfloats_values[makeName(jets_label,pref,"Pt")][j]<< " eta "<< eta<< " phi "<< phi << " e "<< energy <<std::endl;
      float jecscale = vfloats_values[makeName(jets_label,pref,"jecFactor0")][j];
      float area = vfloats_values[makeName(jets_label,pref,"jetArea")][j];

      // if(pt>0){
      // 	ptCorr = smearPt(pt, genpt, eta, syst);
	
      // 	float unc = jetUncertainty(ptCorr,eta,syst);
      // 	//cout << "genpt? "<< genpt <<" pt ? "<< pt<<" ptcorr? "<< ptCorr<<"unc? "<< unc<<endl;
      // 	ptCorr = ptCorr * (1 + unc);
      // 	energyCorr = energy * (1 + unc);
      // }
		if(pt>0){
			TLorentzVector jetUncorr;
	
			jetUncorr.SetPtEtaPhiE(pt,eta,phi,energy);
	
//			std::cout<<"Check #1: "<<jetUncorr.Pt()<<"\t"<<jetUncorr.Eta()<<"\t"<<jetUncorr.Energy()<<std::endl;
	
	//	  cout << "jet "<< j <<" standard pt "<<pt<< " eta "<< eta << " zero correction "<< jecscale<<endl;;
			jetUncorr= jetUncorr*jecscale;

//			std::cout<<"Check #2: "<<jetUncorr.Pt()<<"\t"<<jetUncorr.Eta()<<"\t"<<jetUncorr.Energy()<<std::endl;

			DUnclusteredMETPx+=jetUncorr.Pt()*cos(phi);
			DUnclusteredMETPy+=jetUncorr.Pt()*sin(phi);

			if(changeJECs){
				jecCorr->setJetEta(jetUncorr.Eta());
				jecCorr->setJetPt(jetUncorr.Pt());
				jecCorr->setJetA(area);
				jecCorr->setRho(Rho);
				jecCorr->setNPV(nPV);
				double recorr =  jecCorr->getCorrection();
				jetUncorr = jetUncorr *recorr;
	  //	cout << " recorrection "<<recorr << " corrected Pt "<< jetUncorr.Pt()<< " eta "<< jetUncorr.Eta()<<endl;
				pt = jetUncorr.Pt();
				eta = jetUncorr.Eta();
				energy = jetUncorr.Energy();
				phi = jetUncorr.Phi();
			}

//			std::cout<<"Check #3: "<<jetUncorr.Pt()<<"\t"<<jetUncorr.Eta()<<"\t"<<jetUncorr.Energy()<<std::endl;

	
			smearfact = smear(pt, genpt, eta, syst);
			ptCorr = pt * smearfact;
			energyCorr = energy * smearfact;
		
//			std::cout<<"Check #4: "<<ptCorr<<"\t"<<eta<<"\t"<<energyCorr<<std::endl;

			float unc = jetUncertainty(ptCorr,eta,syst);
	//	cout << "genpt? "<< genpt <<" pt ? "<< pt<<" ptcorr? "<< ptCorr<<"unc? "<< unc<<endl;
			if(unc != 0 && !useMETNoHF){ // for noHFMet we use the uncertainty from the NoHFJet set.
				corrMetPx -=unc*(cos(phi)*ptCorr);
				corrMetPy -=unc*(sin(phi)*ptCorr);
			}
	
			if (syst.find("jer__")!=std::string::npos && (fabs(eta)<3.0 || (!useMETNoHF))){//For JER propagation to met we use standard jets, as reclusteded ones don't have genparticle information
				corrMetPx -=pt * (smearfact-1) * cos(phi);
				corrMetPy -=pt * (smearfact-1) * sin(phi);  
			}
		
			ptCorr = ptCorr * (1 + unc);
			energyCorr = energyCorr * (1 + unc);
		}

    	if(syst.find("unclusteredMet")!= std::string::npos && !useMETNoHF){
	
			DUnclusteredMETPx=metZeroCorrX+DUnclusteredMETPx;
			DUnclusteredMETPy=metZeroCorrY+DUnclusteredMETPy;
	
			double signmet = 1.0; 
			if(syst.find("down")!=std::string::npos) signmet=-1.0;
			corrMetPx -=signmet*DUnclusteredMETPx*0.1;
			corrMetPy -=signmet*DUnclusteredMETPy*0.1;
		}


//      ptCorr = ptCorr;
//      energyCorr = energyCorr;
//      float csv = vfloats_values[makeName(jets_label,pref,"CSVv2")][j]; 
	  float cmva = vfloats_values[makeName(jets_label,pref,"CMVAv2")][j];
      float partonFlavour = vfloats_values[makeName(jets_label,pref,"PartonFlavour")][j];
      int flavor = int(partonFlavour);
      //      vfloats_values[jets_label+"_CorrPt"][j]=ptCorr;
      ///      vfloats_values[jets_label+"_CorrE"][j]=energyCorr;
      vfloats_values[jets_label+"_CorrPt"][j]=ptCorr;
      vfloats_values[jets_label+"_CorrE"][j]=energyCorr;
      vfloats_values[jets_label+"_CorrEta"][j]=eta;
      vfloats_values[jets_label+"_CorrPhi"][j]=phi;
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP
      bool isCMVAT = cmva  > 0.9432;
      bool isCMVAM = cmva  > 0.4432 ;
      bool isCMVAL = cmva  > -0.5884;
      vfloats_values[jets_label+"_IsCMVAT"][j]=isCMVAT;
      vfloats_values[jets_label+"_IsCMVAM"][j]=isCMVAM;
      vfloats_values[jets_label+"_IsCMVAL"][j]=isCMVAL;
      
//      std::cout<<"Flavor: "<<flavor<<"\tDiscriminator: "<<cmva<<"\tIsCMVAT? "<<int(isCMVAT)<<"\tIsCMVAM? "<<int(isCMVAM)<<"\tIsCMVAL? "<<int(isCMVAL)<<std::endl;
      
/*      float bsf = getScaleFactor(ptCorr,eta,partonFlavour,"noSyst");
      float bsfup = getScaleFactor(ptCorr,eta,partonFlavour,"up");
      float bsfdown = getScaleFactor(ptCorr,eta,partonFlavour,"down");
      
      vfloats_values[jets_label+"_BSF"][j]=bsf;
      vfloats_values[jets_label+"_BSFUp"][j]=bsfup;
      vfloats_values[jets_label+"_BSFDown"][j]=bsfdown;
*/
      //***** JET ID ****      
      bool passesID = true;
      
      if(!(jecscale*energy > 0))passesID = false;
      else{
        double enZero=energy*jecscale;
	
//	float nDau = vfloats_values[makeName(jets_label,pref,"numberOfDaughters")][j];
	  float mu_frac = vfloats_values[makeName(jets_label,pref,"MuonEnergy")][j]/enZero;
	  float chMulti = vfloats_values[makeName(jets_label,pref,"chargedMultiplicity")][j];
	  float chHadEnFrac = vfloats_values[makeName(jets_label,pref,"chargedHadronEnergyFrac")][j];///enZero;
	  float chEmEnFrac = vfloats_values[makeName(jets_label,pref,"chargedEmEnergyFrac")][j];///enZero;
	  float neuEmEnFrac = vfloats_values[makeName(jets_label,pref,"neutralEmEnergyFrac")][j];///enZero;
	  float neuHadEnFrac = vfloats_values[makeName(jets_label,pref,"neutralHadronEnergyFrac")][j];///enZero;
	  float neuMulti = vfloats_values[makeName(jets_label,pref,"neutralMultiplicity")][j];

	
	// recommended at 8 TeV
	//passesID =  (nDau >1.0 && fabs(eta) < 4.7 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99) ) && neuEmEnFrac<0.99 && neuHadEnFrac <0.99 && mu_frac < 0.8);

	// temp fix for HF issues
	//passesID =  (nDau >1.0 && fabs(eta) < 4.7 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99 && neuEmEnFrac<0.99 && neuHadEnFrac <0.99) ) && mu_frac < 0.8);

	// new recommendations from 14/08
//	if (fabs(eta)<=3.0) passesID =  (nDau >1.0 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99) ) && neuEmEnFrac<0.99 && neuHadEnFrac <0.99);
//	else passesID = (neuMulti > 10 && neuEmEnFrac < 0.9 );

	vfloats_values[jets_label+"_JetID_muonEnergyFraction"][j]=mu_frac;
	vfloats_values[jets_label+"_JetID_chargedMultiplicity"][j]=chMulti;
	vfloats_values[jets_label+"_JetID_chargedHadronEnergyFraction"][j]=chHadEnFrac;
	vfloats_values[jets_label+"_JetID_chargedEmEnergyFraction"][j]=chEmEnFrac;
	vfloats_values[jets_label+"_JetID_neutralEmEnergyFraction"][j]=neuEmEnFrac;
	vfloats_values[jets_label+"_JetID_neutralHadronEnergyFraction"][j]=neuHadEnFrac;
	vfloats_values[jets_label+"_JetID_neutralMultiplicity"][j]=neuMulti;
	vfloats_values[jets_label+"_JetID_numberOfDaughters"][j]=chMulti+neuMulti;

/*	
	std::cout << "### JET ID ###" << std::endl;
	std::cout <<"enZero "<<enZero<< std::endl;
	std::cout << "ndau " << chMulti+neuMulti << std::endl;
	std::cout << "chMulti " << chMulti << std::endl;
	std::cout << "neuMulti " << neuMulti << std::endl;
	std::cout << "mu_frac " << mu_frac << std::endl;
	std::cout << "chHadEnFrac " << chHadEnFrac << std::endl;
	std::cout << "chEmEnFrac " << chEmEnFrac << std::endl;
	std::cout << "neuEmEnFrac " << neuEmEnFrac << std::endl;
	std::cout << "neuHadEnFrac " << neuHadEnFrac << std::endl;
	std::cout << "passes loose id " << int(passesID) << std::endl;
*/		
      }

      vfloats_values[jets_label+"_PassesID"][j]=(float)passesID;
//      std::cout<<"Check 1"<<std::endl;
      
      //remove overlap with tight electrons/muons
      double minDR=9999;
      double minDRtight=9999;
      double minDRThrEl=0.4;
      double minDRThrMu=0.4;
      bool passesDR=true;
      bool passesDRtight=true;

      for (size_t e = 0; e < (size_t)electrons.size(); ++e){
		minDR = min(minDR,deltaR(math::PtEtaPhiELorentzVector(electrons.at(e).Pt(),electrons.at(e).Eta(),electrons.at(e).Phi(),electrons.at(e).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
		if(minDR<minDRThrEl)passesDR = false;
      }
      for (size_t m = 0; m < (size_t)muons.size(); ++m){
		minDR = min(minDR,deltaR(math::PtEtaPhiELorentzVector(muons.at(m).Pt(),muons.at(m).Eta(),muons.at(m).Phi(),muons.at(m).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
	//minDR = min(minDR,deltaR(muons.at(m) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
		if(minDR<minDRThrMu)passesDR = false;
      }
      
      for (size_t m = 0; m < (size_t)muons_t.size(); ++m){
		minDRtight = min(minDRtight,deltaR(math::PtEtaPhiELorentzVector(muons_t.at(m).Pt(),muons_t.at(m).Eta(),muons_t.at(m).Phi(),muons_t.at(m).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
		if(minDRtight<minDRThrMu)passesDRtight = false;
      }

      vfloats_values[jets_label+"_MinDR"][j]=minDR;
      vfloats_values[jets_label+"_MinDRtight"][j]=minDRtight;      
      vfloats_values[jets_label+"_PassesDR"][j]=(float)passesDR;
      vfloats_values[jets_label+"_PassesDRtight"][j]=(float)passesDRtight;

      //vfloats_values[jets_label+"_IsTight"][j]=0.0;
      //vfloats_values[jets_label+"_IsLoose"][j]=0.0;
	
      if(passesID) vfloats_values[jets_label+"_IsLoose"][j]=1.0;
      if(passesDR) vfloats_values[jets_label+"_PassesDR"][j]=1.0;
      if(passesDRtight) vfloats_values[jets_label+"_PassesDRtight"][j]=1.0;

      if( passesID && passesDR) vfloats_values[jets_label+"_IsLoose"][j]=1.0;
      for (size_t ji = 0; ji < (size_t)jetScanCuts.size(); ++ji){
		stringstream j_n;
		double jetval = jetScanCuts.at(ji);
		j_n << "Cut" <<jetval;
		bool passesCut = ( ptCorr > jetval && fabs(eta) < 4.7);

	
		if(!passesID || !passesCut || !passesDR) continue;
		if(ji==0){
			vfloats_values[jets_label+"_IsTight"][j]=1.0;
			TLorentzVector jet;
			jet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
			jets.push_back(jet);

	  //double MCTagEFFiciency(int flavor); 
	  //  double TagScaleFactor(int flavor, string syst);    
   
	  //  jsfscsvt_b_tag_up;
	  //    jsfscsvt_b_tag_down, 
	  //    jsfscsvt_mistag_up, 
	  //    jsfscsvt_mistag_down;

	  //    partonFlavour = vfloats_values[makeName(jets_label,pref,"PartonFlavour")][j];    
	  
		}
//	      cout << " syst "<< syst<< " jet "<< j << " pt "<< ptCorr <<"cut "<< jetScanCuts.at(ji)<< " extra jet with pt "<< ptCorr<< "eventNJets before is" << float_values["Event_nJets"+j_n.str()]<< " csv "<< csv<< " isCSVM? "<< isCSVM<<endl;
		if(passesCut)	float_values["Event_nJets"+j_n.str()]+=1;
//		std::cout<<  "after: "<< float_values["Event_nJets"+j_n.str()]<<std::endl;

		if(!isData && passesCut){

//		std::cout<<"b-tagging SF method starts"<<std::endl;
//		std::cout<<ptCorr<<"\t"<<eta<<"\t"<<flavor<<std::endl;
	  	
	  	if(fabs(eta)>2.4) continue;
	  
	  	double cmvateff = MCTagEfficiency("cmvat",flavor, ptCorr, eta);
	  	double cmvaleff = MCTagEfficiency("cmval",flavor,ptCorr, eta);
	  	double cmvameff = MCTagEfficiency("cmvam",flavor,ptCorr, eta);
	
//		std::cout<<ptCorr<<"\t"<<eta<<"\t"<<flavor<<endl;
	  
//		std::cout<<"CMVAv2T eff: "<<cmvateff<<"\tCMVAv2M eff: "<<cmvameff<<"\tCMVAv2L eff: "<<cmvaleff<<std::endl;	

	  	
	  	double sfcmval = TagScaleFactor("cmval",flavor,"noSyst",ptCorr, eta);
	  	double sfcmvam = TagScaleFactor("cmvam",flavor,"noSyst",ptCorr, eta);
	  	double sfcmvat = TagScaleFactor("cmvat",flavor,"noSyst",ptCorr, eta);
	   
	  
//		std::cout<<"Tight: "<<sfcmvat <<"\tMedium: "<<sfcmvam<<"\tLoose: "<<sfcmval<<std::endl;

      	double sfcmvat_mistag_up = TagScaleFactor("cmvat",flavor,"mistag_up",ptCorr, eta);
	  	double sfcmval_mistag_up = TagScaleFactor("cmval",flavor,"mistag_up",ptCorr, eta);
	  	double sfcmvam_mistag_up = TagScaleFactor("cmvam",flavor,"mistag_up",ptCorr, eta);

	  	double sfcmvat_mistag_down = TagScaleFactor("cmvat",flavor,"mistag_down",ptCorr, eta);
	  	double sfcmval_mistag_down = TagScaleFactor("cmval",flavor,"mistag_down",ptCorr, eta);
	  	double sfcmvam_mistag_down = TagScaleFactor("cmvam",flavor,"mistag_down",ptCorr, eta);

	  	double sfcmvat_b_tag_down = TagScaleFactor("cmvat",flavor,"btag_down",ptCorr, eta);
	  	double sfcmval_b_tag_down = TagScaleFactor("cmval",flavor,"btag_down",ptCorr, eta);
	  	double sfcmvam_b_tag_down = TagScaleFactor("cmvam",flavor,"btag_down",ptCorr, eta);

	  	double sfcmvat_b_tag_up = TagScaleFactor("cmvat",flavor,"btag_up",ptCorr, eta);
	  	double sfcmval_b_tag_up = TagScaleFactor("cmval",flavor,"btag_up",ptCorr, eta);
	  	double sfcmvam_b_tag_up = TagScaleFactor("cmvam",flavor,"btag_up",ptCorr, eta);
      
      
	  	jsfscmvat.push_back(BTagWeight::JetInfo(cmvateff, sfcmvat));
	  	jsfscmval.push_back(BTagWeight::JetInfo(cmvaleff, sfcmval));
	  	jsfscmvam.push_back(BTagWeight::JetInfo(cmvameff, sfcmvam));

	  	jsfscmvat_mistag_up.push_back(BTagWeight::JetInfo(cmvateff, sfcmvat_mistag_up));
	  	jsfscmval_mistag_up.push_back(BTagWeight::JetInfo(cmvaleff, sfcmval_mistag_up));
	  	jsfscmvam_mistag_up.push_back(BTagWeight::JetInfo(cmvameff, sfcmvam_mistag_up));


	  	jsfscmvat_b_tag_up.push_back(BTagWeight::JetInfo(cmvateff, sfcmvat_b_tag_up));
	  	jsfscmval_b_tag_up.push_back(BTagWeight::JetInfo(cmvaleff, sfcmval_b_tag_up));
	  	jsfscmvam_b_tag_up.push_back(BTagWeight::JetInfo(cmvameff, sfcmvam_b_tag_up));

	  	jsfscmvat_mistag_down.push_back(BTagWeight::JetInfo(cmvateff, sfcmvat_mistag_down));
	  	jsfscmval_mistag_down.push_back(BTagWeight::JetInfo(cmvaleff, sfcmval_mistag_down));
	  	jsfscmvam_mistag_down.push_back(BTagWeight::JetInfo(cmvameff, sfcmvam_mistag_down));

	  	jsfscmvat_b_tag_down.push_back(BTagWeight::JetInfo(cmvateff, sfcmvat_b_tag_down));
	  	jsfscmval_b_tag_down.push_back(BTagWeight::JetInfo(cmvaleff, sfcmval_b_tag_down));
	  	jsfscmvam_b_tag_down.push_back(BTagWeight::JetInfo(cmvameff, sfcmvam_b_tag_down));
	
	}

	//if(isCSVT && passesCut && fabs(eta) < 2.4) {
	if(isCMVAT && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  float_values["Event_nCMVATJets"+j_n.str()]+=1.0;
	  ncmvat_tags +=1;
	}

	//if(isCSVL && passesCut && fabs(eta) < 2.4) { 
	if(isCMVAL && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  ncmval_tags +=1; float_values["Event_nCMVALJets"+j_n.str()]+=1;
	}

	//if(isCSVM && passesCut && fabs(eta) < 2.4) { 
	if(isCMVAM && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  float_values["Event_nCMVAMJets"+j_n.str()]+=1.0; ncmvam_tags +=1;		
	}

/*	  if(ji==0){
	    ncmvam_tags +=1;
	    //    cout << " jet "<< j<< " isBJET "<<endl;
	    //TLorentzVector bjet;
	    //bjet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
	    //bjets.push_back(bjet);
	    //mapBJets[bjetidx]=j;
	    //++bjetidx;
	  }
*/
	  //	  if(ji==0){
	    //	    cout << " jet "<< j<< " isBJET "<<endl;
	    //	    TLorentzVector bjet;
	    //  bjet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
	    //	    bjets.push_back(bjet);
	  //	  }
//	}
	
//	if(isCMVAL && passesCut && abs(eta) < 2.4) float_values["Event_nCMVALJets"+j_n.str()]+=1;
//	std::cout<<  "after: "<< float_values["Event_nJets"+j_n.str()]<<"\t"<<float_values["Event_nCMVATJets"+j_n.str()]<<std::endl;
   }
 }
  
// std::cout<<"Ending Jets"<<std::endl;
    
    if(useMETNoHF){
      DUnclusteredMETPx=0.0;
      DUnclusteredMETPy=0.0;
      for(int j = 0;j < max_instances[jetsnohf_label] ;++j){

		string pref = obj_to_pref[jetsnohf_label];
		float pt = vfloats_values[makeName(jetsnohf_label,pref,"Pt")][j];
	//      if(pt<0)continue;
		float eta = vfloats_values[makeName(jetsnohf_label,pref,"Eta")][j];
		float phi = vfloats_values[makeName(jetsnohf_label,pref,"Phi")][j];
		float energy = vfloats_values[makeName(jetsnohf_label,pref,"E")][j];
		float ptCorr = pt;
	//	float energyCorr = -9999;
	//	float smearfact = -9999;
	//	cout << "j is " <<j<< "label "<< jetsnohf_label << " maxinstances "<< max_instances[jetsnohf_label]<< "size "<< sizes[jetsnohf_label]<< " pt "<< vfloats_values[makeName(jetsnohf_label,pref,"Pt")][j]<< " eta "<< eta<< " phi "<< phi << " e "<< energy <<endl;
		float jecscale = vfloats_values[makeName(jetsnohf_label,pref,"jecFactor0")][j];
		float area = vfloats_values[makeName(jetsnohf_label,pref,"jetArea")][j];
	
		if(pt>0){
	  		TLorentzVector jetUncorr;
	  
	  		jetUncorr.SetPtEtaPhiE(pt,eta,phi,energy);
	  
	  //	  cout << "jet "<< j <<" standard pt "<<pt<< " eta "<< eta << " zero correction "<< jecscale<<endl;;
	  		jetUncorr= jetUncorr*jecscale;
	  //	  cout << " uncorrected "<<jetUncorr.Pt()<<" ch jecs? "<< changeJECs<<endl;
	  //	  cout << " corrmetPx? "<<corrMetPx<<" Py? "<< corrMetPy<<endl;
	  		if(jetUncorr.Pt() > 10){
	    		DUnclusteredMETPx+=jetUncorr.Pt()*cos(phi);
	    		DUnclusteredMETPy+=jetUncorr.Pt()*sin(phi);
	  		}
	  //	  DUnclusteredMETPx=0.1*jetUncorr.Pt()*cos(phi);
	  //	  DUnclusteredMETPy=0.1*jetUncorr.Pt()*sin(phi);
	  		if(changeJECs){
	    		jecCorr->setJetEta(jetUncorr.Eta());
	    		jecCorr->setJetPt(jetUncorr.Pt());
	    		jecCorr->setJetA(area);
	    		jecCorr->setRho(Rho);
	    		jecCorr->setNPV(nPV);
	    
	    		double recorr =  jecCorr->getCorrection();
	    		jetUncorr = jetUncorr *recorr;
	    //	  cout << " recorrection "<<recorr << " corrected Pt "<< jetUncorr.Pt()<< " eta "<< jetUncorr.Eta()<<endl;
	    		pt = jetUncorr.Pt();
	    		eta = jetUncorr.Eta();
	    		energy = jetUncorr.Energy();
	   			phi = jetUncorr.Phi();
	  		}
	  
	  		float unc = jetUncertainty(pt,eta,syst);
	  		if(unc != 0){
	    		corrMetPx -=unc*(cos(phi)*ptCorr);
	    		corrMetPy -=unc*(sin(phi)*ptCorr);
	  		}
		}
      }
      
		if(syst.find("unclusteredMet")!= std::string::npos ){
	//	cout << "metZeroCorrX is "<< metZeroCorrX << " y "<< metZeroCorrY << " metX "<< metPx << " metPy "<<metPy << " DuncX"<< DUnclusteredMETPx << " y "<< DUnclusteredMETPy<<endl;

	  	DUnclusteredMETPx=metZeroCorrX+DUnclusteredMETPx;
	  	DUnclusteredMETPy=metZeroCorrY+DUnclusteredMETPy;
	  	double signmet = 1.0; 
	  	if(syst.find("down")!=std::string::npos) signmet=-1.0;
  		corrMetPx -=signmet*DUnclusteredMETPx*0.1;
  		corrMetPy -=signmet*DUnclusteredMETPy*0.1;
		}
    }
    
    //MET corrections
    metPx+=corrMetPx; metPy+=corrMetPy; // add JEC/JER contribution
    
    //Correcting the pt
    float metptCorr = sqrt(metPx*metPx + metPy*metPy); 
    vfloats_values[met_label+"_CorrPt"][0]=metptCorr;

    //Correcting the phi
    float metphiCorr = metphi;
    if(metPx<0){
      if(metPy>0)metphiCorr = atan(metPy/metPx)+3.141592;
      if(metPy<0)metphiCorr = atan(metPy/metPx)-3.141592;
    }
    else  metphiCorr = (atan(metPy/metPx));
    
    vfloats_values[met_label+"_CorrPhi"][0]=metphiCorr;

//	std::cout<<"Ending MET"<<std::endl;
    //BTagging part
    if(!isData && doBTagSF){

///  CMVAT
      //0 tags
      b_weight_cmvat_0_tags = b_cmvat_0_tags.weight(jsfscmvat, ncmvat_tags);  
      b_weight_cmvat_0_tags_mistag_up = b_cmvat_0_tags.weight(jsfscmvat_mistag_up, ncmvat_tags);  
      b_weight_cmvat_0_tags_mistag_down = b_cmvat_0_tags.weight(jsfscmvat_mistag_down, ncmvat_tags);  
      b_weight_cmvat_0_tags_b_tag_up = b_cmvat_0_tags.weight(jsfscmvat_b_tag_up, ncmvat_tags);  
      b_weight_cmvat_0_tags_b_tag_down = b_cmvat_0_tags.weight(jsfscmvat_b_tag_down, ncmvat_tags);  
      
      //1 tag
      b_weight_cmvat_1_tag = b_cmvat_1_tag.weight(jsfscmvat, ncmvat_tags);  
      b_weight_cmvat_1_tag_mistag_up = b_cmvat_1_tag.weight(jsfscmvat_mistag_up, ncmvat_tags);  
      b_weight_cmvat_1_tag_mistag_down = b_cmvat_1_tag.weight(jsfscmvat_mistag_down, ncmvat_tags);  
      b_weight_cmvat_1_tag_b_tag_up = b_cmvat_1_tag.weight(jsfscmvat_b_tag_up, ncmvat_tags);  
      b_weight_cmvat_1_tag_b_tag_down = b_cmvat_1_tag.weight(jsfscmvat_b_tag_down, ncmvat_tags);  
      //cout <<"w1t check: is"<< b_weight_cmvat_1_tag<<endl;
      
      //2 tags
      b_weight_cmvat_2_tags = b_cmvat_2_tags.weight(jsfscmvat, ncmvat_tags);  
      b_weight_cmvat_2_tags_mistag_up = b_cmvat_2_tags.weight(jsfscmvat_mistag_up, ncmvat_tags);  
      b_weight_cmvat_2_tags_mistag_down = b_cmvat_2_tags.weight(jsfscmvat_mistag_down, ncmvat_tags);  
      b_weight_cmvat_2_tags_b_tag_up = b_cmvat_2_tags.weight(jsfscmvat_b_tag_up, ncmvat_tags);  
      b_weight_cmvat_2_tags_b_tag_down = b_cmvat_2_tags.weight(jsfscmvat_b_tag_down, ncmvat_tags);  

/// CMVAM    
      //0 tags
      b_weight_cmvam_0_tags = b_cmvam_0_tags.weight(jsfscmvam, ncmvam_tags);  
      b_weight_cmvam_0_tags_mistag_up = b_cmvam_0_tags.weight(jsfscmvam_mistag_up, ncmvam_tags);  
      b_weight_cmvam_0_tags_mistag_down = b_cmvam_0_tags.weight(jsfscmvam_mistag_down, ncmvam_tags);  
      b_weight_cmvam_0_tags_b_tag_up = b_cmvam_0_tags.weight(jsfscmvam_b_tag_up, ncmvam_tags);  
      b_weight_cmvam_0_tags_b_tag_down = b_cmvam_0_tags.weight(jsfscmvam_b_tag_down, ncmvam_tags);  
      
      //1 tag
      b_weight_cmvam_1_tag = b_cmvam_1_tag.weight(jsfscmvam, ncmvam_tags);  
      b_weight_cmvam_1_tag_mistag_up = b_cmvam_1_tag.weight(jsfscmvam_mistag_up, ncmvam_tags);  
      b_weight_cmvam_1_tag_mistag_down = b_cmvam_1_tag.weight(jsfscmvam_mistag_down, ncmvam_tags);  
      b_weight_cmvam_1_tag_b_tag_up = b_cmvam_1_tag.weight(jsfscmvam_b_tag_up, ncmvam_tags);  
      b_weight_cmvam_1_tag_b_tag_down = b_cmvam_1_tag.weight(jsfscmvam_b_tag_down, ncmvam_tags);  
      //cout <<"w1t check: is"<< b_weight_cmvam_1_tag<<endl;
      
      //2 tags
      b_weight_cmvam_2_tags = b_cmvam_2_tags.weight(jsfscmvam, ncmvam_tags);  
      b_weight_cmvam_2_tags_mistag_up = b_cmvam_2_tags.weight(jsfscmvam_mistag_up, ncmvam_tags);  
      b_weight_cmvam_2_tags_mistag_down = b_cmvam_2_tags.weight(jsfscmvam_mistag_down, ncmvam_tags);  
      b_weight_cmvam_2_tags_b_tag_up = b_cmvam_2_tags.weight(jsfscmvam_b_tag_up, ncmvam_tags);  
      b_weight_cmvam_2_tags_b_tag_down = b_cmvam_2_tags.weight(jsfscmvam_b_tag_down, ncmvam_tags);  

/// CMVAL
      //0 tags
      b_weight_cmval_0_tags = b_cmval_0_tags.weight(jsfscmval, ncmval_tags);  
      b_weight_cmval_0_tags_mistag_up = b_cmval_0_tags.weight(jsfscmval_mistag_up, ncmval_tags);  
      b_weight_cmval_0_tags_mistag_down = b_cmval_0_tags.weight(jsfscmval_mistag_down, ncmval_tags);  
      b_weight_cmval_0_tags_b_tag_up = b_cmval_0_tags.weight(jsfscmval_b_tag_up, ncmval_tags);  
      b_weight_cmval_0_tags_b_tag_down = b_cmval_0_tags.weight(jsfscmval_b_tag_down, ncmval_tags);  
      
      //1 tag
      b_weight_cmval_1_tag = b_cmval_1_tag.weight(jsfscmval, ncmval_tags);  
      b_weight_cmval_1_tag_mistag_up = b_cmval_1_tag.weight(jsfscmval_mistag_up, ncmval_tags);  
      b_weight_cmval_1_tag_mistag_down = b_cmval_1_tag.weight(jsfscmval_mistag_down, ncmval_tags);  
      b_weight_cmval_1_tag_b_tag_up = b_cmval_1_tag.weight(jsfscmval_b_tag_up, ncmval_tags);  
      b_weight_cmval_1_tag_b_tag_down = b_cmval_1_tag.weight(jsfscmval_b_tag_down, ncmval_tags);  
      //cout <<"w1t check: is"<< b_weight_cmval_1_tag<<endl;
      
      //2 tags
      b_weight_cmval_2_tags = b_cmval_2_tags.weight(jsfscmval, ncmval_tags);  
      b_weight_cmval_2_tags_mistag_up = b_cmval_2_tags.weight(jsfscmval_mistag_up, ncmval_tags);  
      b_weight_cmval_2_tags_mistag_down = b_cmval_2_tags.weight(jsfscmval_mistag_down, ncmval_tags);  
      b_weight_cmval_2_tags_b_tag_up = b_cmval_2_tags.weight(jsfscmval_b_tag_up, ncmval_tags);  
      b_weight_cmval_2_tags_b_tag_down = b_cmval_2_tags.weight(jsfscmval_b_tag_down, ncmval_tags);  


      float_values["Event_bWeight0CMVAL"]=b_weight_cmval_0_tags;
      float_values["Event_bWeight1CMVAL"]=b_weight_cmval_1_tag;
      float_values["Event_bWeight2CMVAL"]=b_weight_cmval_2_tags;
      
      float_values["Event_bWeight0CMVAM"]=b_weight_cmvam_0_tags;
      float_values["Event_bWeight1CMVAM"]=b_weight_cmvam_1_tag;
      float_values["Event_bWeight2CMVAM"]=b_weight_cmvam_2_tags;
      
      float_values["Event_bWeight0CMVAT"]=b_weight_cmvat_0_tags;
      float_values["Event_bWeight1CMVAT"]=b_weight_cmvat_1_tag;
      float_values["Event_bWeight2CMVAT"]=b_weight_cmvat_2_tags;
      
      float_values["Event_bWeight0CMVAL"]=b_weight_cmval_0_tags;
      float_values["Event_bWeight1CMVAL"]=b_weight_cmval_1_tag;
      float_values["Event_bWeight2CMVAL"]=b_weight_cmval_2_tags;
      
      float_values["Event_bWeight0CMVAM"]=b_weight_cmvam_0_tags;
      float_values["Event_bWeight1CMVAM"]=b_weight_cmvam_1_tag;
      float_values["Event_bWeight2CMVAM"]=b_weight_cmvam_2_tags;
      
      float_values["Event_bWeight0CMVAT"]=b_weight_cmvat_0_tags;
      float_values["Event_bWeight1CMVAT"]=b_weight_cmvat_1_tag;
      float_values["Event_bWeight2CMVAT"]=b_weight_cmvat_2_tags;

      //Mistag
      float_values["Event_bWeightMisTagUp0CMVAL"]=b_weight_cmval_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CMVAL"]=b_weight_cmval_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CMVAL"]=b_weight_cmval_2_tags_mistag_up;
      
      float_values["Event_bWeightMisTagUp0CMVAM"]=b_weight_cmvam_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CMVAM"]=b_weight_cmvam_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CMVAM"]=b_weight_cmvam_2_tags_mistag_up;
    
      float_values["Event_bWeightMisTagUp0CMVAT"]=b_weight_cmvat_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CMVAT"]=b_weight_cmvat_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CMVAT"]=b_weight_cmvat_2_tags_mistag_up;
      
      
      float_values["Event_bWeightMisTagDown0CMVAL"]=b_weight_cmval_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CMVAL"]=b_weight_cmval_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CMVAL"]=b_weight_cmval_2_tags_mistag_down;
      
      float_values["Event_bWeightMisTagDown0CMVAM"]=b_weight_cmvam_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CMVAM"]=b_weight_cmvam_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CMVAM"]=b_weight_cmvam_2_tags_mistag_down;
      
      float_values["Event_bWeightMisTagDown0CMVAT"]=b_weight_cmvat_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CMVAT"]=b_weight_cmvat_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CMVAT"]=b_weight_cmvat_2_tags_mistag_down;

      //Btag
      float_values["Event_bWeightBTagUp0CMVAL"]=b_weight_cmval_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CMVAL"]=b_weight_cmval_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CMVAL"]=b_weight_cmval_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagUp0CMVAM"]=b_weight_cmvam_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CMVAM"]=b_weight_cmvam_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CMVAM"]=b_weight_cmvam_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagUp0CMVAT"]=b_weight_cmvat_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CMVAT"]=b_weight_cmvat_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CMVAT"]=b_weight_cmvat_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagDown0CMVAL"]=b_weight_cmval_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CMVAL"]=b_weight_cmval_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CMVAL"]=b_weight_cmval_2_tags_b_tag_down;
      
      float_values["Event_bWeightBTagDown0CMVAM"]=b_weight_cmvam_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CMVAM"]=b_weight_cmvam_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CMVAM"]=b_weight_cmvam_2_tags_b_tag_down;
      
      float_values["Event_bWeightBTagDown0CMVAT"]=b_weight_cmvat_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CMVAT"]=b_weight_cmvat_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CMVAT"]=b_weight_cmvat_2_tags_b_tag_down;      
    }
    
//    std::cout<<"Ending Btagging SFs"<<std::endl;
     
    if(useLHE){
      //LHE and luminosity weights:
      float LHE_weight = lhes->hepeup().XWGTUP;
      float LHEWeightSign = LHE_weight/fabs(LHE_weight);
	  float LHEhepNUP = lhes->hepeup().NUP;
//	  std::cout<<"############ LHE Info #################\nLHEWeightSign: "<<LHEWeightSign<<"\thepNUP: "<<LHEhepNUP<<std::endl;		
      float_values["Event_LHEWeightSign"]=LHEWeightSign;
      float_values["Event_LHEWeight"]=LHE_weight;
	  float_values["Event_LHEhepNUP"]=LHEhepNUP;

      if(doTopPtReweight){
		std::vector<TLorentzVector> gentop;
		std::vector<TLorentzVector> genantitop;

		gentop.clear();
		genantitop.clear();				

		for( size_t i=0;i<(size_t)LHEhepNUP;++i){
//    		cout << " particle number " << i << endl;
			int id = lhes->hepeup().IDUP[i];
			float px = lhes->hepeup().PUP[i][0];
			float py = lhes->hepeup().PUP[i][1];
			float pz = lhes->hepeup().PUP[i][2];
			float energy = lhes->hepeup().PUP[i][3];

//    		float mass = lhes->hepeup().PUP[i][4];      
//    		if(abs (id) == 6 )  cout << " px is"<< px << " py "<< py << " pz "<< pz << " e "<<energy<<endl;    

			TLorentzVector vec;
			math::XYZTLorentzVector part = math::XYZTLorentzVector(px, py, pz, energy);
			float pt = part.pt();
			float phi = part.phi();
			float eta = part.eta();
		    if(abs (id) == 6 )  std::cout << " pt is"<< pt << " eta "<< eta << " phi "<< phi << " e "<<energy<<std::endl;          
			if(pt>0){
				vec.SetPtEtaPhiE(pt, eta, phi, energy);
				if(id == 6 ) gentop.push_back(vec);
				if(id == -6 ) genantitop.push_back(vec);
			}
		}

		if(gentop.size()==1){	
			float_values["Event_LHE_T_Pt"]=gentop.at(0).Pt();
			float_values["Event_LHE_T_Eta"]=gentop.at(0).Eta();
			float_values["Event_LHE_T_Phi"]=gentop.at(0).Phi();
			float_values["Event_LHE_T_E"]=gentop.at(0).Energy();
			float_values["Event_LHE_T_size"]=1.0;
		}
		
		if(genantitop.size()==1){
			float_values["Event_LHE_Tbar_Pt"]=genantitop.at(0).Pt();
			float_values["Event_LHE_Tbar_Eta"]=genantitop.at(0).Eta();
			float_values["Event_LHE_Tbar_Phi"]=genantitop.at(0).Phi();
			float_values["Event_LHE_Tbar_E"]=genantitop.at(0).Energy();
			float_values["Event_LHE_Tbar_size"]=1.0;			
		}
		double ptT_LHE = gentop.at(0).Pt();
		double ptTbar_LHE = genantitop.at(0).Pt();				
		double topPtwgt_LHE = getTopPtWeight(ptT_LHE,ptTbar_LHE);
		float_values["LHEtopPtWeight"]=topPtwgt_LHE;
		
		for( size_t j=0; j<prunedGenParticles->size(); ++j){
			const reco::GenParticle & genIt = (*prunedGenParticles)[j];
//			int absid=abs(genIt.pdgId());
			
			if(abs(genIt.pdgId())==6 && genIt.isLastCopy()){
				genPartpt=genIt.pt();
				genParteta= genIt.eta();
				genPartphi= genIt.phi();
				genPartE= genIt.energy();
				genPartId= genIt.pdgId();	
			}
			TLorentzVector vec;
			if(genPartpt>0){
				vec.SetPtEtaPhiE(genPartpt, genParteta, genPartphi, genPartE);
				if(genPartId == 6) gentop.push_back(vec);
				if(genPartId == -6) genantitop.push_back(vec);
			}
		}	
		
		if(gentop.size()==2){
				float_values["Event_gen_T_Pt"]=gentop.at(1).Pt();
				float_values["Event_gen_T_Eta"]=gentop.at(1).Eta();
				float_values["Event_gen_T_Phi"]=gentop.at(1).Phi();
				float_values["Event_gen_T_E"]=gentop.at(1).Energy();
				float_values["Event_gen_T_size"]=1.0;
		}
		
		if(genantitop.size()==2){
			float_values["Event_gen_Tbar_Pt"]=genantitop.at(1).Pt();
			float_values["Event_gen_Tbar_Eta"]=genantitop.at(1).Eta();
			float_values["Event_gen_Tbar_Phi"]=genantitop.at(1).Phi();
			float_values["Event_gen_Tbar_E"]=genantitop.at(1).Energy();
			float_values["Event_gen_Tbar_size"]=1.0;
		}
		double ptT_gen = gentop.at(1).Pt();
		double ptTbar_gen = genantitop.at(1).Pt();				
		double topPtwgt_gen = getTopPtWeight(ptT_gen, ptTbar_gen);
		float_values["topPtWeight"]=topPtwgt_gen;
			
	  }		  		  
    }
    
    float weightLumi = crossSection/originalEvents;
    if(!isData) float_values["Event_weight"]=weightLumi;
  
  
    //Part 3: filling the additional variables
    //Reset event weights/#objects
    if(useLHEWeights){
      getEventLHEWeights();
    }
    
    if(addLHAPDFWeights){
      getEventPdf();
    }
    		

    if(addPV){
      float nGoodPV = 0.0;
      for (size_t v = 0; v < pvZ->size();++v){
		bool isGoodPV = (fabs(pvZ->at(v)) < 24.0 && pvNdof->at(v) > 4.0 && pvRho->at(v) <2.0);
		if (isGoodPV)nGoodPV+=1.0;
      }	
      float_values["Event_nGoodPV"]=(float)(nGoodPV);
      float_values["Event_nPV"]=(float)(nPV);
    }

    //    if(useMETFilters){
    //      getMETFilters();
    //    }
    
//    float_values["Event_passesHBHE"]=(float)(*HBHE);
//    float_values["Event_passesHBHEIso"]=(float)(*HBHEIso);

    //technical event information
    int_values["Event_EventNumber"]=(*eventNumber);
    //float_values["Event_EventNumber"]=(float)(*eventNumber);
    float_values["Event_LumiBlock"]=*lumiBlock;
    float_values["Event_RunNumber"]=*runNumber;

    //    std::cout << " event ntight muons " << float_values["Event_nTightMuons"]<< std::endl;
    //    std::cout << " event ntight electrons " << float_values["Event_nTightElectrons"]<< std::endl;
    //    cout << "before filling jets label "<< jets_label << " maxinstances "<< max_instances[jets_label]<< "size "<< sizes[jets_label]<<endl;  
    trees[syst]->Fill();
    //Reset event weights/#objects
    string nameshortv= "Event";
    vector<string> extravars = additionalVariables(nameshortv);
    for(size_t addv = 0; addv < extravars.size();++addv){
      string name = nameshortv+"_"+extravars.at(addv);
      //      std::cout << " resetting variable "<< name<< " before "<< float_values[name];
      //      bool isTriggerVar
      //      if(isTriggerVar) continue;
      float_values[name]=-1;
      //      std::cout << " after "<< Float_values[name]<<endl;
    }
  }

  /*
  for(int t = 0;t < max_instances[boosted_tops_label] ;++t){
    vfloats_values[boosted_tops_label+"_nCSVM"][t]=0;
    vfloats_values[boosted_tops_label+"_nJ"][t]=0;
  }
  */

  //treesBase->Fill(); 
}

string DMAnalysisTreeMaker::makeBranchName(string label, string pref, string var){
  string outVar = var;
  size_t prefPos=outVar.find(pref);
  size_t prefLength = pref.length();
  outVar.replace(prefPos,prefLength,label+"_");
  return outVar;
}

string DMAnalysisTreeMaker::makeName(string label,string pref,string var){
  return label+"_"+var;
  //  string outVar = var;
  //  size_t prefPos=outVar.find(pref);
  //  size_t prefLength = pref.length();
  //  std::cout << " outvar is "<< outVar<< " prefpos "<< prefPos<< " length "<< prefLength<< endl;
  //  std::cout << "it is " << label+"_"+var<<endl;
  //  outVar.replace(prefPos,prefLength,label+"_");

  //  outVar.replace(prefPos,prefLength,label+"_");
  //  return outVar;
  //  return makeBranchName(label,pref,var);
  //  return pref+var+"_"+label;
}

vector<string> DMAnalysisTreeMaker::additionalVariables(string object){
  vector<string> addvar;
  bool ismuon=object.find("muon")!=std::string::npos;
  bool iselectron=object.find("electron")!=std::string::npos;
  bool ismet=object.find("met")!=std::string::npos;
  bool isjet=object.find("jet")!=std::string::npos && object.find("AK4")!=std::string::npos;
  bool isak8=object.find("jet")!=std::string::npos && object.find("AK8")!=std::string::npos && object.find("sub")==std::string::npos;
  bool isak8subjet=object.find("jet")!=std::string::npos && object.find("AK8")!=std::string::npos && object.find("sub")!=std::string::npos;
  bool isevent=object.find("Event")!=std::string::npos;
  //bool isResolvedTopHad=object.find("resolvedTopHad")!=std::string::npos;
  //bool isResolvedTopSemiLep=object.find("resolvedTopSemiLep")!=std::string::npos;
  
  if(ismuon || iselectron){
    //addvar.push_back("SFTrigger");
    //addvar.push_back("SFReco");
    //addvar.push_back("isQCD");
    //    addvar.push_back("isTightOffline");
    //    addvar.push_back("isLooseOffline");
  }
  if(iselectron){
    //addvar.push_back("PassesDRmu");
  }
  if(ismet){
    //    addvar.push_back("Pt");
    //    addvar.push_back("Eta");
    //    addvar.push_back("Phi");
    //    addvar.push_back("E");
    addvar.push_back("CorrPt");
    addvar.push_back("CorrPhi");
  }
  if(isjet){
    addvar.push_back("CorrPt");
    //    addvar.push_back("CorrEta");
    //    addvar.push_back("CorrPhi");
    addvar.push_back("CorrE");
    addvar.push_back("MinDR");
    addvar.push_back("IsCMVAT");
    addvar.push_back("IsCMVAM");
    addvar.push_back("IsCMVAL");
    //    addvar.push_back("BSF");
    //    addvar.push_back("BSFUp");
    //    addvar.push_back("BSFDown");
    addvar.push_back("PassesID");
    addvar.push_back("PassesDR");
    addvar.push_back("PassesDRtight");
    addvar.push_back("CorrMass");
    addvar.push_back("IsTight");
    addvar.push_back("IsLoose");
    //    addvar.push_back("CorrNJets");
    //    addvar.push_back("CorrPartonFlavour");

//    addvar.push_back("JetID_numberOfDaughters");
    addvar.push_back("JetID_muonEnergyFraction");
    addvar.push_back("JetID_chargedMultiplicity");
    addvar.push_back("JetID_chargedHadronEnergyFraction");
    addvar.push_back("JetID_chargedEmEnergyFraction");
    addvar.push_back("JetID_neutralEmEnergyFraction");
    addvar.push_back("JetID_neutralHadronEnergyFraction");
    addvar.push_back("JetID_neutralMultiplicity");

  }
  if(isak8){
    addvar.push_back("CorrPt");
    addvar.push_back("CorrE");
    addvar.push_back("isType1");
    addvar.push_back("isType2");
    addvar.push_back("TopPt");
    addvar.push_back("TopEta");
    addvar.push_back("TopPhi");
    addvar.push_back("TopE");
    addvar.push_back("TopMass");
    addvar.push_back("TopWMass");
    addvar.push_back("nJ");
    addvar.push_back("nCMVAM");
  }  
  if(isak8subjet){
    ;//    addvar.push_back("CorrPt");
  }
  /*
  if(isResolvedTopHad ){
    addvar.push_back("Pt");    addvar.push_back("Eta");    addvar.push_back("Phi");    addvar.push_back("E"); addvar.push_back("Mass"); 
    addvar.push_back("WMass"); addvar.push_back("BMPhi");  addvar.push_back("WMPhi");  addvar.push_back("TMPhi");  addvar.push_back("WBPhi");
    addvar.push_back("IndexB");    addvar.push_back("IndexJ1");    addvar.push_back("IndexJ2");
  }

  if(isResolvedTopSemiLep ){
    addvar.push_back("Pt");    addvar.push_back("Eta");    addvar.push_back("Phi");    addvar.push_back("E"); addvar.push_back("Mass");   
    addvar.push_back("MT");    addvar.push_back("LBMPhi");    addvar.push_back("LMPhi");    addvar.push_back("LBPhi");     addvar.push_back("BMPhi");  addvar.push_back("TMPhi"); 
    addvar.push_back("IndexL");    addvar.push_back("LeptonFlavour");    addvar.push_back("IndexB");
  }
  */
  if(isevent){
    //addvar.push_back("weight");
    addvar.push_back("nTightMuons");
    addvar.push_back("nSoftMuons");
    addvar.push_back("nLooseMuons");
    addvar.push_back("nTightElectrons");
    addvar.push_back("nMediumElectrons");
    addvar.push_back("nLooseElectrons");
    addvar.push_back("nVetoElectrons");
	addvar.push_back("EventNumber");
    addvar.push_back("LumiBlock");
    addvar.push_back("RunNumber");
    
    //addvar.push_back("nElectronsSF");
    //addvar.push_back("mt");
    //addvar.push_back("Mt2w");
    //addvar.push_back("category");
    //addvar.push_back("nMuonsSF");
    //addvar.push_back("nCSVTJets");
    //addvar.push_back("nCSVMJets");
    //addvar.push_back("nCSVLJets");
    //addvar.push_back("nTightJets");
    //addvar.push_back("nLooseJets");
    //addvar.push_back("nType1TopJets");
    //addvar.push_back("nType2TopJets");
 
    if(!isData && doBTagSF){
		addvar.push_back("bWeight0CMVAT");
		addvar.push_back("bWeight1CMVAT");
		addvar.push_back("bWeight2CMVAT");
		addvar.push_back("bWeight0CMVAM");
		addvar.push_back("bWeight1CMVAM");
		addvar.push_back("bWeight2CMVAM");

		addvar.push_back("bWeight0CMVAL");
		addvar.push_back("bWeight1CMVAL");
		addvar.push_back("bWeight2CMVAL");

		addvar.push_back("bWeightMisTagDown0CMVAT");
		addvar.push_back("bWeightMisTagDown1CMVAT");
		addvar.push_back("bWeightMisTagDown2CMVAT");

		addvar.push_back("bWeightMisTagDown0CMVAM");
		addvar.push_back("bWeightMisTagDown1CMVAM");
		addvar.push_back("bWeightMisTagDown2CMVAM");

		addvar.push_back("bWeightMisTagDown0CMVAL");
		addvar.push_back("bWeightMisTagDown1CMVAL");
		addvar.push_back("bWeightMisTagDown2CMVAL");

		addvar.push_back("bWeightMisTagUp0CMVAT");
		addvar.push_back("bWeightMisTagUp1CMVAT");
		addvar.push_back("bWeightMisTagUp2CMVAT");

		addvar.push_back("bWeightMisTagUp0CMVAM");
		addvar.push_back("bWeightMisTagUp1CMVAM");
		addvar.push_back("bWeightMisTagUp2CMVAM");

		addvar.push_back("bWeightMisTagUp0CMVAL");
		addvar.push_back("bWeightMisTagUp1CMVAL");
		addvar.push_back("bWeightMisTagUp2CMVAL");

		addvar.push_back("bWeightBTagUp0CMVAT");
		addvar.push_back("bWeightBTagUp1CMVAT");
		addvar.push_back("bWeightBTagUp2CMVAT");
		addvar.push_back("bWeightBTagUp0CMVAM");
		addvar.push_back("bWeightBTagUp1CMVAM");
		addvar.push_back("bWeightBTagUp2CMVAM");

		addvar.push_back("bWeightBTagUp0CMVAL");
		addvar.push_back("bWeightBTagUp1CMVAL");
		addvar.push_back("bWeightBTagUp2CMVAL");

		addvar.push_back("bWeightBTagDown0CMVAT");
		addvar.push_back("bWeightBTagDown1CMVAT");
		addvar.push_back("bWeightBTagDown2CMVAT");

		addvar.push_back("bWeightBTagDown0CMVAM");
		addvar.push_back("bWeightBTagDown1CMVAM");
		addvar.push_back("bWeightBTagDown2CMVAM");

		addvar.push_back("bWeightBTagDown0CMVAL");
		addvar.push_back("bWeightBTagDown1CMVAL");
		addvar.push_back("bWeightBTagDown2CMVAL");
	}
    
    if(!isData && useLHE){
		addvar.push_back("LHEWeightSign");
		addvar.push_back("LHEWeight");
		addvar.push_back("LHEhepNUP");
    
		if(doTopPtReweight){
			addvar.push_back("LHEtopPtWeight");
			addvar.push_back("topPtWeight");

			addvar.push_back("Event_LHE_T_size");
			addvar.push_back("Event_LHE_T_Pt");
			addvar.push_back("Event_LHE_T_Eta");
			addvar.push_back("Event_LHE_T_Phi");
			addvar.push_back("Event_LHE_T_E");
			addvar.push_back("Event_LHE_Tbar_size");
			addvar.push_back("Event_LHE_Tbar_Pt");
			addvar.push_back("Event_LHE_Tbar_Eta");
			addvar.push_back("Event_LHE_Tbar_Phi");
			addvar.push_back("Event_LHE_Tbar_E");

			addvar.push_back("Event_gen_T_size");
			addvar.push_back("Event_gen_T_Pt");
			addvar.push_back("Event_gen_T_Eta");
			addvar.push_back("Event_gen_T_Phi");
			addvar.push_back("Event_gen_T_E");
			addvar.push_back("Event_gen_Tbar_size");
			addvar.push_back("Event_gen_Tbar_Pt");
			addvar.push_back("Event_gen_Tbar_Eta");
			addvar.push_back("Event_gen_Tbar_Phi");
			addvar.push_back("Event_gen_Tbar_E");
		}	
	}	
	
	if(!isData){
		addvar.push_back("mu_eff");
		addvar.push_back("mu_eff_up");
		addvar.push_back("mu_eff_down");
	}
	
    if(addPV){
      addvar.push_back("nPV");
      addvar.push_back("nGoodPV");
    }
    
    /*
    for (size_t j = 0; j < (size_t)jetScanCuts.size(); ++j){
      stringstream j_n;
      double jetval = jetScanCuts.at(j);
      j_n << "Cut" <<jetval;
      
      addvar.push_back("nJets"+j_n.str());
      addvar.push_back("nCSVTJets"+j_n.str());
      addvar.push_back("nCSVMJets"+j_n.str());
      addvar.push_back("nCSVLJets"+j_n.str());
      addvar.push_back("bWeight1CSVTWeight"+j_n.str());
      addvar.push_back("bWeight2CSVTWeight"+j_n.str());
      addvar.push_back("bWeight1CSVMWeight"+j_n.str());
      addvar.push_back("bWeight2CSVMWeight"+j_n.str());
      addvar.push_back("bWeight1CSVLWeight"+j_n.str());
      addvar.push_back("bWeight2CSVLWeight"+j_n.str());
      addvar.push_back("bWeight0CSVLWeight"+j_n.str());
      addvar.push_back("bWeight0CSVLWeight"+j_n.str());
    }
    */

    if(useLHEWeights){
      for (size_t w = 0; w <= (size_t)maxWeights; ++w)  {
	stringstream w_n;
	w_n << w;
	addvar.push_back("LHEWeight"+w_n.str());
	//	cout << " weight # " << w << " test "<< "LHEWeight"+w_n.str()<< endl; 
	//addvar.push_back(("LHEWeight"+w_n.str())+"ID");
      }
    }
    if(addLHAPDFWeights){
      for (size_t p = 1; p <= (size_t)maxPdf; ++p)  {
	//cout << " pdf # " << pdf_weights[p - 1] << " test "<< endl; 
	stringstream w_n;
	w_n << p;
	addvar.push_back("PDFWeight" + w_n.str());
      }
    }
    if(useMETFilters){
//      for (size_t lt = 0; lt < metFilters.size(); ++lt)  {
//		string trig = metFilters.at(lt);
//		addvar.push_back("passes"+trig);
//      }
      addvar.push_back("passesMETFilters");
//      addvar.push_back("passesHBHE");
//      addvar.push_back("passesHBHEIso");
    }

    if(useTriggers){
		for (size_t lt = 0; lt < leptonicTriggers.size(); ++lt)  {
			std::string trig = leptonicTriggers.at(lt);
			addvar.push_back("passes"+trig);
			addvar.push_back("prescale"+trig);
		}			

		for(size_t ht = 0; ht < hadronicTriggers.size(); ++ht){
			std::string trig = hadronicTriggers.at(ht);
			addvar.push_back("passes"+trig);
			addvar.push_back("prescale"+trig);
		}
	  								
		addvar.push_back("passesLeptonicTriggers");
		addvar.push_back("passesHadronicTriggers");
    }

//------------ Soureek adding PU info -----------------    
    if(doPU_){
      addvar.push_back("puWeight");
      addvar.push_back("puWeightUp");
      addvar.push_back("puWeightDown");
      addvar.push_back("nTruePU");
    }
  }
      
  return addvar;
}

void DMAnalysisTreeMaker::initializePdf(string central, string varied){

    if(central == "CT") {  LHAPDF::initPDFSet(1, "cteq66.LHgrid"); }
    if(central == "CT10") {  LHAPDF::initPDFSet(1, "CT10.LHgrid"); }
    if(central == "CT10f4") {  LHAPDF::initPDFSet(1, "CT10f4.LHgrid"); }
    if(central == "NNPDF") { LHAPDF::initPDFSet(1, "NNPDF22_100.LHgrid");  }
    if(central == "MSTW") { LHAPDF::initPDFSet(1, "MSTW2008nlo68cl.LHgrid");  }

    if(varied == "CT") {  LHAPDF::initPDFSet(2, "cteq66.LHgrid"); maxPdf = 44; }
    if(varied == "CT10") {  LHAPDF::initPDFSet(2, "CT10.LHgrid"); maxPdf = 52; }
    if(varied == "CT10f4") {  LHAPDF::initPDFSet(2, "CT10f4.LHgrid"); maxPdf = 52; }
    if(varied == "NNPDF") { LHAPDF::initPDFSet(2, "NNPDF22_100.LHgrid");  maxPdf = 100; }
    if(varied == "MSTW") { LHAPDF::initPDFSet(2, "MSTW2008nlo68cl.LHgrid"); maxPdf = 40; }
}


bool DMAnalysisTreeMaker::getMETFilters(){

  bool METFilterAND=true;
  for(size_t mf =0; mf< metFilters.size();++mf){
    string fname = metFilters.at(mf);
    for(size_t bt = 0; bt < metNames->size();++bt){
      std::string tname = metNames->at(bt);
//      cout << "test tname "<<endl;
      if(tname.find(fname)!=std::string::npos){
		METFilterAND = METFilterAND && (metBits->at(bt)>0);
		float_values["Event_passes"+fname]=metBits->at(bt);
      }
    }
  }
  float_values["Event_passesMETFilters"]=(float)METFilterAND;
  return METFilterAND;
}

bool DMAnalysisTreeMaker::getEventTriggers(){
  bool leptonOR=false,hadronOR=false;
  for(size_t lt =0; lt< leptonicTriggers.size();++lt){
    std::string lname = leptonicTriggers.at(lt);
    for(size_t bt = 0; bt < triggerNames->size();++bt){
      std::string tname = triggerNames->at(bt);
      if(tname.find(lname)!=std::string::npos){
//		std::cout<<lname<<"\t"<<tname<<std::endl;
		leptonOR = leptonOR || (triggerBits->at(bt)>0);
		float_values["Event_passes"+lname]=triggerBits->at(bt);
		float_values["Event_prescale"+lname]=triggerPrescales->at(bt);
      }
    }
  }
  for(size_t ht =0; ht< hadronicTriggers.size();++ht){
    string hname = hadronicTriggers.at(ht);
    for(size_t bt = 0; bt < triggerNames->size();++bt){
      std::string tname = triggerNames->at(bt);
      if(tname.find(hname)!=std::string::npos){
		hadronOR = hadronOR || (triggerBits->at(bt)>0);
		float_values["Event_passes"+hname]=triggerBits->at(bt);
		float_values["Event_prescale"+hname]=triggerPrescales->at(bt);
      }
    }
  }
  float_values["Event_passesLeptonicTriggers"]=(float)leptonOR;
  float_values["Event_passesHadronicTriggers"]=(float)hadronOR;
  return (leptonOR || hadronOR);
}



void DMAnalysisTreeMaker::getPUSF(){
  puWeight=(float) LumiWeights_.weight(nTruePU);
  puWeightUp = (float) LumiWeightsUp_.weight(nTruePU);
  puWeightDown = (float) LumiWeightsDown_.weight(nTruePU);
  // std::cout<<"nTruePU: "<<nTruePU<<"\tnPV: "<<nPV<<std::endl;
  // std::cout<<"pileUp weight: "<<puWeight<<"\tpileUp weight Up: "<<puWeightUp<<"\tpileUp weight Down: "<<puWeightDown<<std::endl;
  float_values["Event_puWeight"]=puWeight;
  float_values["Event_puWeightUp"]=puWeightUp; 
  float_values["Event_puWeightDown"]=puWeightDown;
  float_values["Event_nTruePU"]=(float)nTruePU;
}


void DMAnalysisTreeMaker::getEventPdf(){

  if (genprod->pdf()->id.first == 21 || genprod->pdf()->id.second == 21) return;

  std::cout << " getting pdf "<<endl;

  double scalePDF = genprod->pdf()->scalePDF;
  double x1 =  genprod->pdf()->x.first;
  double x2 =  genprod->pdf()->x.second;
  int id1 =  genprod->pdf()->id.first;
  int id2 =  genprod->pdf()->id.second;

  std::cout << " maxpdf "<< maxPdf << " accessing x1 " << x1<< " id1 " << id1<<std::endl;
  std::cout << " maxpdf "<< maxPdf << " accessing x2 " << x2<< " id2 " << id2<<std::endl;
  cout << "scalePDF " << scalePDF << endl;

  LHAPDF::usePDFMember(1, 0);
  double xpdf1 = LHAPDF::xfx(1, x1, scalePDF, id1);
  double xpdf2 = LHAPDF::xfx(1, x2, scalePDF, id2);
  double w0 = xpdf1 * xpdf2;
  int maxPDFCount = maxPdf;

  cout << "xpdf1 " << xpdf1 << endl;
  cout << "xpdf2 " << xpdf2 << endl;

  cout << "weight " << w0 << endl;

  std::cout << "entering pdf loop" <<std::endl;
  for (int p = 1; p <= maxPdf; ++p)
    {
      
      if ( p > maxPDFCount ) continue;
      LHAPDF::usePDFMember(2, p);
      double xpdf1_new = LHAPDF::xfx(2, x1, scalePDF, id1);
      double xpdf2_new = LHAPDF::xfx(2, x2, scalePDF, id2);
      double pweight = xpdf1_new * xpdf2_new / w0;
      stringstream w_n;
      w_n << p;

      cout << "xpdf1 new " << xpdf1_new << endl;
      cout << "xpdf2 new " << xpdf2_new << endl;

      cout << "index " << p << " pweight " << pweight << endl;

      float_values["PDFWeight"+w_n.str()]= pweight;
    }
  
}


void DMAnalysisTreeMaker::getEventLHEWeights(){
  //  std::cout << " in weight "<<endl;
//  std::string weightID;
  size_t wgtsize=  lhes->weights().size();
  //  std::cout << "weight size "<< wgtsize<<endl;
  for (size_t i = 0; i <  wgtsize; ++i)  {
    if (i<= (size_t)maxWeights){ 
      stringstream w_n;
      w_n << i;

      float ww = (float)lhes->weights().at(i).wgt;
//	  weightID=(lhes->weights().at(i).id.data()).str();
      std::cout<<"Name: "<<lhes->weights().at(i).id.data()<<"\t Weight: "<<ww<<std::endl; 
      //      cout << "ww # " << i<< "is "<<ww <<endl;
      //      cout <<" floatval before "<< float_values["Event_LHEWeight"+w_n.str()]<<endl;

      float_values["Event_LHEWeight"+w_n.str()]= ww;

      //      cout <<" floatval after "<< float_values["Event_LHEWeight"+w_n.str()]<<endl;

    }
    //else cout << "WARNING! there are " << wgtsize << " weights, and you accomodated for only "<< maxWeights << " weights, check your configuration file/your lhe!!!"<<endl;
  }
  
}


// double DMAnalysisTreeMaker::smearPt(double ptCorr, double genpt, double eta, string syst){
//   double resolScale = resolSF(fabs(eta), syst);
//   double smear =1.0;
//   if(genpt>0) smear = std::max((double)(0.0), (double)(ptCorr + (ptCorr - genpt) * resolScale) / ptCorr);
//   return ptCorr * smear;
// }

double DMAnalysisTreeMaker::smear(double pt, double genpt, double eta, string syst){
  double resolScale = resolSF(fabs(eta), syst);
  double smear =1.0;
  if(genpt>0) smear = std::max((double)(0.0), (double)(genpt + (pt - genpt) * resolScale) / pt);
  return  smear;
}

double DMAnalysisTreeMaker::getTopPtWeight(double ptT, double ptTbar, bool extrap){
  if((ptT>0.0 && ptTbar>0.0) ){
//    if (extrap || (ptT<=400.0 && ptTbar <=400.0)){
      //      double a = 0.156;// 7/8 TeV
      //      double b = -0.00137;//
      double a = 0.0615;//
      double b = -0.0005;//
      double sfT = exp(a+b*ptT);
      double sfTbar = exp(a+b*ptTbar);
	  return sqrt(sfT*sfTbar); 
//    }
  }
  return 1.0;
}
/*
 // 8 TeV scale factors
double DMAnalysisTreeMaker::resolSF(double eta, string syst)
{
  double fac = 0.;
  if (syst == "jer__up")fac = 1.;
  if (syst == "jer__down")fac = -1.;
  if (eta <= 0.5)                       return 0.079 + (0.026 * fac);
  else if ( eta > 0.5 && eta <= 1.1 )   return 0.099 + (0.028 * fac);
  else if ( eta > 1.1 && eta <= 1.7 )   return 0.121 + (0.029 * fac);
  else if ( eta > 1.7 && eta <= 2.3 )   return 0.208 + (0.046 * fac);
  else if ( eta > 2.3 && eta <= 2.8 )   return 0.254 + (0.062 * fac);
  else if ( eta > 2.8 && eta <= 3.2 )   return 0.395 + (0.063 * fac);
  else if ( eta > 3.2 && eta <= 5.0 )   return 0.056 + (0.191 * fac);
  return 0.1;
}
*/

// 80X Data/MC JER SFs
double DMAnalysisTreeMaker::resolSF(double eta, string syst)
{
  double fac = 0.;
  if (syst == "jer__up")fac = 1.;
  if (syst == "jer__down")fac = -1.;
  if( eta <= 0.5 )                      return 1.109 + (0.008 * fac);		
  else if ( eta > 0.5 && eta <= 0.8 )   return 1.138 + (0.013 * fac);
  else if ( eta > 0.8 && eta <= 1.1 )   return 1.114 + (0.013 * fac);
  else if ( eta > 1.1 && eta <= 1.3 )   return 1.123 + (0.024 * fac);
  else if ( eta > 1.3 && eta <= 1.7 )   return 1.084 + (0.011 * fac);
  else if ( eta > 1.7 && eta <= 1.9 )   return 1.082 + (0.035 * fac);
  else if ( eta > 1.9 && eta <= 2.1 )   return 1.140 + (0.047 * fac);
  else if ( eta > 2.1 && eta <= 2.3 )   return 1.067 + (0.053 * fac);
  else if ( eta > 2.3 && eta <= 2.5 )   return 1.177 + (0.041 * fac);
  else if ( eta > 2.5 && eta <= 2.8 )   return 1.364 + (0.039 * fac);
  else if ( eta > 2.8 && eta <= 3.0 )   return 1.857 + (0.071 * fac);
  else if ( eta > 3.0 && eta <= 3.2 )   return 1.328 + (0.022 * fac);
  else                                  return 1.16 + (0.029 * fac);
}

double DMAnalysisTreeMaker::jetUncertainty(double ptCorr, double eta, string syst)
{
  double fac;	
  if(ptCorr<0)return ptCorr;
  if(TString(syst).Contains(TString("jes"))){ 
	if(TString(syst).Contains(TString("up"))) fac = 1.;
    if (TString(syst).Contains(TString("down")))fac = -1.;
    jecUnc[syst]->setJetEta(eta);
    jecUnc[syst]->setJetPt(ptCorr);
    double JetCorrection = jecUnc[syst]->getUncertainty(true);
    return JetCorrection*fac;
  }
  return 0.0;
}

//double DMAnalysisTreeMaker::getScaleFactor(double ptCorr,double etaCorr,double partonFlavour, string syst){
//  return 1.0;
//}

bool DMAnalysisTreeMaker::isInVector(std::vector<std::string> v, std::string s){
  for(size_t i = 0;i<v.size();++i){
    //    std::cout << " label is " << s << " vector i-th element  "<< v.at(i)<<" are they equal? "<< (v.at(i)==s) << " is v in s? "<< (s.find(v.at(i))!=std::string::npos)<<endl;
    if(v.at(i)==s)return true;
    //    if(s.find(v.at(i))!=std::string::npos)return true;
  }
  return false;
}

//BTag weighter
bool DMAnalysisTreeMaker::BTagWeight::filter(int t)
{
  return (t >= minTags && t <= maxTags);
}

float DMAnalysisTreeMaker::BTagWeight::weight(vector<JetInfo> jetTags, int tags)
{
  if (!filter(tags))
    {
      //   std::cout << "nThis event should not pass the selection, what is it doing here?" << std::endl;
      return 0;
    }
  int njetTags = jetTags.size();
  //cout<< " njettags "<< njetTags<<endl;
  int comb = 1 << njetTags;
  float pMC = 0;
  float pData = 0;
  for (int i = 0; i < comb; i++)
    {
      float mc = 1.;
      float data = 1.;
      int ntagged = 0;
      for (int j = 0; j < njetTags; j++)
        {
	  bool tagged = ((i >> j) & 0x1) == 1;
	  if (tagged)
            {
	      ntagged++;
	      mc *= jetTags[j].eff;
	      data *= jetTags[j].eff * jetTags[j].sf;
            }
	  else
            {
	      mc *= (1. - jetTags[j].eff);
	      data *= (1. - jetTags[j].eff * jetTags[j].sf);
            }
        }

      if (filter(ntagged))
        {
	  //std::cout << mc << " " << data << endl;
	  pMC += mc;
	  pData += data;
        }
    }

  if (pMC == 0) return 0;
  return pData / pMC;
}

/*double DMAnalysisTreeMaker::MCTagEfficiency(string algo, int flavor, double pt){

	double eff=1.0;
/*	if (abs(flavor) ==5){
    if(algo=="csvt") return 0.38;
    if(algo=="csvm") return 0.58;
    if(algo=="csvl") return 0.755;
  }

  if (abs(flavor) ==4){
    if(algo=="csvt") return 0.015;
    if(algo=="csvm") return 0.08;
    if(algo=="csvl") return 0.28;
  }

  if (abs(flavor) !=4 && abs(flavor) !=5){
    if(algo=="csvt") return 0.0008;
    if(algo=="csvm") return 0.007;
    if(algo=="csvl") return 0.079;
  }
  return 1.0;

/// MC Tag Efficiencies calculated for cMVAv2 algorithm based on BTV-15-001
	if(abs(flavor) == 5){
		if(algo == "cmvat"){ 
			if(pt>=30.0 && pt<160.0) eff = 0.127 + 0.01578*pt - 2.126*pow(10,-4)*pt*pt + 1.273*pow(10,-6)*pt*pt*pt - 2.88*pow(10,-9)*pt*pt*pt*pt;
			if(pt>=160.0) eff = 0.634 - 6.74*pow(10,-4)*pt + 2.69*pow(10,-7)*pt*pt;
		}
		if(algo == "cmvam"){
			if(pt>=30.0 && pt<175) eff = 0.421 + 0.0107*pt - 1.314*pow(10,-4)*pt*pt + 7.268*pow(10,-7)*pt*pt*pt - 1.523*pow(10,-9)*pt*pt*pt*pt;
			if(pt>=175.0) eff = 0.79 - 3.17*pow(10,-4)*pt + 1.24*pow(10,-7)*pt*pt;
		}
		if(algo == "cmval"){
			if(pt>=30.0 && pt<150.0) eff = 0.707+5.6*pow(10,-3)*pt - 6.27*pow(10,-5)*pt*pt + 3.10*pow(10,-7)*pt*pt*pt - 5.63*pow(10,-10)*pt*pt*pt*pt;
			if(pt>=150.0) eff = 0.906 - 6.39*pow(10,-5)*pt + 4.11*pow(10,-8)*pt*pt;
		}	
	}
	
	if(abs(flavor) == 4){
		if(algo == "cmvat"){ 
			if(pt>=30.0 && pt<240.0) eff = 0.024 + 5.27*pow(10,-4)*pt - 3.72*pow(10,-6)*pt*pt + 9.87*pow(10,-9)*pt*pt*pt - 8.83*pow(10,-12)*pt*pt*pt*pt;
			if(pt>=240.0) eff = 0.044;
		}
		if(algo == "cmvam"){
			if(pt>=30.0 && pt<170.0) eff = 0.13 + 1.48*pow(10,-3)*pt - 1.00*pow(10,-5)*pt*pt + 2.65*pow(10,-8)*pt*pt*pt - 2.36*pow(10,-11)*pt*pt*pt*pt;
			if(pt>=170.0) eff = 0.20;
		}
		if (algo == "cmval"){
			if(pt>=30.0 && pt<205.0) eff = 0.40 + 1.23*pow(10,-3)*pt - 4.60*pow(10,-6)*pt*pt + 5.71*pow(10,-9)*pt*pt*pt;
			if(pt>=205.0) eff = 0.478 + 1.573*pow(10,-4)*pt;
		}
	}
	
	if(abs(flavor)!=4 && abs(flavor)!=5){
		if(algo == "cmvat"){ 
			if(pt>=30.0 && pt<130.0) eff= 1.24*pow(10,-3) - 1.27*pow(10,-5)*pt + 1.98*pow(10,-7)*pt*pt - 7.46*pow(10,-10)*pt*pt*pt + 8.35*pow(10,-13)*pt*pt*pt*pt;
			if(pt>=130.0) eff = 1.08*pow(10,-3)+ 3.54*pow(10,-6)*pt;
		}
		if(algo == "cmvam"){
			if(pt>=30.0 && pt<170.0) eff = 9.59*pow(10,-3) - 1.96*pow(10,-5)*pt + 4.53*pow(10,-7)*pt*pt - 1.08*pow(10,-9)*pt*pt*pt + 7.62*pow(10,-13)*pt*pt*pt*pt;
			if(pt>=170.0) eff = 5.07*pow(10,-3) + 6.02*pow(10,-5)*pt - 2.3*pow(10,-8)*pt*pt;
		}
		if (algo == "cmval"){
			if(pt>=30.0 && pt<130.0) eff = 0.124 - 1.0*pow(10,-3)*pt + 1.06*pow(10,-5)*pt*pt - 3.18*pow(10,-8)*pt*pt*pt + 3.13*pow(10,-11)*pt*pt*pt*pt;
			if(pt>=130.0) eff = 0.055 + 4.53*pow(10,-4)*pt -1.6*pow(10,-7)*pt*pt;
		}	
	}
	
	return eff;								
}
*/

double DMAnalysisTreeMaker::MCTagEfficiency(string algo, int flavor, double pt, double eta){
  if(pt < 40)pt = 40.1;
  if (abs(flavor) ==5){
//    if(algo=="csvt") return 0.51;
//    if(algo=="csvm") return 0.71;
//    if(algo=="csvl") return 0.86;
    if(algo=="cmvat") return cmvaeffbt->getEff(fabs(eta),pt);
    if(algo=="cmvam") return cmvaeffbm->getEff(fabs(eta),pt);
    if(algo=="cmval") return cmvaeffbl->getEff(fabs(eta),pt);

  }

  if (abs(flavor) ==4){
//    if(algo=="csvt") return 0.015;
//    if(algo=="csvm") return 0.08;
//    if(algo=="csvl") return 0.28;
    if(algo=="cmvat") return cmvaeffct->getEff(fabs(eta),pt);
    if(algo=="cmvam") return cmvaeffcm->getEff(fabs(eta),pt);
    if(algo=="cmval") return cmvaeffcl->getEff(fabs(eta),pt);
  }

  if (abs(flavor) !=4 && abs(flavor) !=5){
//    if(algo=="csvt") return 0.003;
//    if(algo=="csvm") return 0.02;
//    if(algo=="csvl") return 0.16;
    if(algo=="cmvat") return 0.003; //cmvaeffot->getEff(fabs(eta),pt);
    if(algo=="cmvam") return 0.02;//cmvaeffom->getEff(fabs(eta),pt);
    if(algo=="cmval") return 0.13; //cmvaeffol->getEff(fabs(eta),pt);
  }
  return 1.0;
}

double DMAnalysisTreeMaker::TagScaleFactor(string algo, int flavor, string syst, double pt, double eta){
// source (03/08/16):
// https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation80X/CSVv2_ichep.csv
  	
	
/*	if(algo=="cmvat"){
		if(syst=="noSyst"){
			if(abs(flavor)==5 || abs(flavor)==4){
				if(pt>=30.0 && pt<300.0) return 0.806363+(0.000389927*pt);
				if(pt>=300.0){ ptTmp= 300.0 - 0.0001; return 0.806363+(0.000389927*ptTmp);}
			}
			else{
				if(pt>=20.0 && pt<1000.0) return 0.712074+293.026/(pt*pt);
				if(pt>=1000.0){ ptTmp=1000.0 - 0.0001; return 0.712074+293.026/(ptTmp*ptTmp);}
			}
		}	
		
		if(syst=="btag_up"){
			if(abs(flavor)==5){
				if(pt>=30.0 && pt<50.0) return (0.806363+(0.000389927*pt))+0.036142464727163315;
				if(pt>=50.0 && pt<70.0) return (0.806363+(0.000389927*pt))+0.02133803628385067;
				if(pt>=70.0 && pt<100.0) return (0.806363+(0.000389927*pt))+0.015581912361085415;
				if(pt>=100.0 && pt<140.0) return (0.806363+(0.000389927*pt))+0.018966136500239372;
				if(pt>=140.0 && pt<200.0) return (0.806363+(0.000389927*pt))+0.024859944358468056;
				if(pt>=200.0 && pt<300.0) return (0.806363+(0.000389927*pt))+0.062531806528568268;
				if(pt>=300.0){
					ptTmp=300.0 - 0.0001; 
					return (0.806363+(0.000389927*ptTmp))+ 2 * 0.062531806528568268; // Double uncertainty if pt is out of range
				}		  
			}
			else if(abs(flavor)==4){
				if(pt>=30.0 && pt<50.0) return (0.806363+(0.000389927*pt))+0.07228492945432663;
				if(pt>=50.0 && pt<70.0) return (0.806363+(0.000389927*pt))+0.04267607256770134;
				if(pt>=70.0 && pt<100.0) return (0.806363+(0.000389927*pt))+0.03116382472217083;
				if(pt>=100.0 && pt<140.0) return (0.806363+(0.000389927*pt))+0.037932273000478745;
				if(pt>=140.0 && pt<200.0) return (0.806363+(0.000389927*pt))+0.049719888716936111;
				if(pt>=200.0 && pt<300.0) return (0.806363+(0.000389927*pt))+0.12506361305713665;
				if(pt>=300.0){ 
					ptTmp=300.0 - 0.0001; 
					return (0.806363+(0.000389927*ptTmp))+2 * 0.12506361305713665; // Double uncertainty if pt is out of range
				}
			}
			else return 1.0;			
		}
		
		if(syst=="btag_down"){
			if(abs(flavor)==5){
				if(pt>=30.0 && pt<50.0) return 0.806363+((0.000389927*pt)-0.036142464727163315);
				if(pt>=50.0 && pt<70.0) return 0.806363+((0.000389927*pt)-0.02133803628385067);
				if(pt>=70.0 && pt<100.0) return 0.806363+((0.000389927*pt)-0.015581912361085415);
				if(pt>=100.0 && pt<140.0) return 0.806363+((0.000389927*pt)-0.018966136500239372);
				if(pt>=140.0 && pt<200.0) return 0.806363+((0.000389927*pt)-0.024859944358468056);
				if(pt>=200.0 && pt<300.0) return 0.806363+((0.000389927*pt)-0.062531806528568268);
				if(pt>=300.0){ 
					ptTmp=300.0 - 0.0001; 
					return 0.806363+((0.000389927*pt)- 2 * 0.062531806528568268); // Double uncertainty if pt is out of range

				} 			}
			else if(abs(flavor)==4){
				if(pt>=30.0 && pt<50.0) return 0.806363+((0.000389927*ptTmp)-0.07228492945432663);
				if(pt>=50.0 && pt<70.0) return 0.806363+((0.000389927*ptTmp)-0.04267607256770134);
				if(pt>=70.0 && pt<100.0) return 0.806363+((0.000389927*ptTmp)-0.03116382472217083); 
				if(pt>=100.0 && pt<140.0) return 0.806363+((0.000389927*ptTmp)-0.037932273000478745);
				if(pt>=140.0 && pt<200.0) return 0.806363+((0.000389927*ptTmp)-0.049719888716936111);
				if(pt>=200.0 && pt<300.0) return 0.806363+((0.000389927*ptTmp)-0.12506361305713665);
				if(pt>=300.0){ 
					ptTmp=300.0 - 0.0001; 
					return 0.806363+((0.000389927*ptTmp)-2 * 0.12506361305713665); // Double uncertainty if pt is out of range
				} 
			}
			else return 1.0;
		}
		
		if(syst=="mistag_up"){
			if(abs(flavor)== 5.0 || abs(flavor)== 4.0) return 1.0;
			else{
				if(pt>=20.0 && pt<1000.0) return (0.712074+293.026/(pt*pt))*(1 + (0.145849 + 8.95114e-05*pt + -9.63642e-08*pt*pt));
				if(pt>=1000.0){
					ptTmp=1000.0 - 0.0001; 
					return (0.712074+293.026/(pt*pt))*(1 + 2* (0.145849 + 8.95114e-05*ptTmp + -9.63642e-08*ptTmp*ptTmp)); // Double uncertainty if pt is out of range
				}
			}	
		}
		
		if(syst=="mistag_down"){
			if(abs(flavor)== 5.0 || abs(flavor)== 4.0) return 1.0;
			else{
				if(pt>=20.0 && pt<1000.0) return (0.712074+293.026/(pt*pt))*(1- (0.145849 + 8.95114e-05*pt + -9.63642e-08*pt*pt));
				if(pt>=1000.0){
					ptTmp=1000.0 - 0.0001;
					return (0.712074+293.026/(ptTmp*ptTmp))*(1- 2 * (0.145849 + 8.95114e-05*ptTmp + -9.63642e-08*pt*ptTmp)); // Double uncertainty if pt is out of range
				}	
			}			
		}	
	}		
	return 1.0;					
*/

	if(algo == "cmval"){
		if(syst ==  "noSyst") {
			if(abs(flavor)==5){return readerCMVALoose->eval_auto_bounds("central",BTagEntry::FLAV_B, eta, pt);    }
			if(abs(flavor)==4){return readerCMVALoose->eval_auto_bounds("central",BTagEntry::FLAV_C, eta, pt);    }
			if(abs(flavor)!=5 && abs(flavor)!=4){ return readerCMVALoose->eval_auto_bounds("central",BTagEntry::FLAV_UDSG, eta, pt);    }    
		}
		
		if(syst ==  "mistag_up") {
			if(abs(flavor)==5){	return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){	return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){	return readerCMVALoose->eval_auto_bounds("up",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}
		
		if(syst ==  "mistag_down") {
			if(abs(flavor)==5){	return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){	return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){	return readerCMVALoose->eval_auto_bounds("down",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}
    
		if(syst ==  "b_tag_up") {
			if(abs(flavor)==5){return readerCMVALoose->eval_auto_bounds("up",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVALoose->eval_auto_bounds("up",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }    
		}
		
		if(syst ==  "b_tag_down") {
			if(abs(flavor)==5){return readerCMVALoose->eval_auto_bounds("down",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVALoose->eval_auto_bounds("down",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }    
		}
	}

	if(algo == "cmvam"){
		if(syst ==  "noSyst") {
			if(abs(flavor)==5){return readerCMVAMedium->eval_auto_bounds("central",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVAMedium->eval_auto_bounds("central",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVAMedium->eval_auto_bounds("central",BTagEntry::FLAV_UDSG, eta, pt);      }
		}
		
		if(syst ==  "mistag_up") {
			if(abs(flavor)==5){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVAMedium->eval_auto_bounds("up",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}
		if(syst ==  "mistag_down") {
			if(abs(flavor)==5){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVAMedium->eval_auto_bounds("down",BTagEntry::FLAV_UDSG, eta, pt);      }   
		}

		if(syst ==  "b_tag_up") {
			if(abs(flavor)==5){return readerCMVAMedium->eval_auto_bounds("up",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVAMedium->eval_auto_bounds("up",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }     
		}
		
		if(syst ==  "b_tag_down") {
			if(abs(flavor)==5){return readerCMVAMedium->eval_auto_bounds("down",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVAMedium->eval_auto_bounds("down",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }    
		}  
	}

	if(algo == "cmvat"){
		if(syst ==  "noSyst") {
			if(abs(flavor)==5){return readerCMVATight->eval_auto_bounds("central",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVATight->eval_auto_bounds("central",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVATight->eval_auto_bounds("central",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}

		if(syst ==  "mistag_up") {
			if(abs(flavor)==5){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVATight->eval_auto_bounds("up",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}
    
		if(syst ==  "mistag_down") {
			if(abs(flavor)==5){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)==4){return 1.00*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return readerCMVATight->eval_auto_bounds("down",BTagEntry::FLAV_UDSG, eta, pt);      }    
		}
    
		if(syst ==  "b_tag_up") {
			if(abs(flavor)==5){return readerCMVATight->eval_auto_bounds("up",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVATight->eval_auto_bounds("up",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }    
		}

		if(syst ==  "b_tag_down") {
			if(abs(flavor)==5){return readerCMVATight->eval_auto_bounds("down",BTagEntry::FLAV_B, eta, pt);      }
			if(abs(flavor)==4){return readerCMVATight->eval_auto_bounds("down",BTagEntry::FLAV_C, eta, pt);      }
			if(abs(flavor)!=5 && abs(flavor)!=4){return 1.0*TagScaleFactor(algo,flavor,"noSyst",pt, eta);      }    
		}
	}
	
	return 1.0;

}			 

float DMAnalysisTreeMaker::BTagWeight::weightWithVeto(vector<JetInfo> jetsTags, int tags, vector<JetInfo> jetsVetoes, int vetoes)
{//This function takes into account cases where you have n b-tags and m vetoes, but they have different thresholds. 
  if (!filter(tags))
    {
      //   std::cout << "nThis event should not pass the selection, what is it doing here?" << std::endl;
      return 0;
    }
  int njets = jetsTags.size();
  if(njets != (int)(jetsVetoes.size()))return 0;//jets tags and vetoes must have same size!
  int comb = 1 << njets;
  float pMC = 0;
  float pData = 0;
  for (int i = 0; i < comb; i++)
    {
      float mc = 1.;
      float data = 1.;
      int ntagged = 0;
      for (int j = 0; j < njets; j++)
        {
	  bool tagged = ((i >> j) & 0x1) == 1;
	  if (tagged)
            {
	      ntagged++;
	      mc *= jetsTags[j].eff;
	      data *= jetsTags[j].eff * jetsTags[j].sf;
            }
	  else
            {
	      mc *= (1. - jetsVetoes[j].eff);
	      data *= (1. - jetsVetoes[j].eff * jetsVetoes[j].sf);
            }
        }

      if (filter(ntagged))
        {
	  //  std::cout << mc << " " << data << endl;
	  pMC += mc;
	  pData += data;
        }
    }

  if (pMC == 0) return 0;
  return pData / pMC;
}

// muon SF based on Muon POG values from Dec15
double DMAnalysisTreeMaker::muonSF(bool isdata, float pt, float eta, int syst) {

  if (isdata) return 1;

  double eff_id = 0;
  double eff_iso = 0;
  double eff_trigger = 0;
  
  // tight id SF 
  if (pt > 20 && pt <= 25) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.9752116203308105 + 0.0030660638813280626 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.9738101959228516 + 0.004502934246978295 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9983288645744324 + 0.002331323348626783 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9877836108207703 + 0.004915740433340289 * syst;
  }

  if (pt > 25 && pt <= 30) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.9848297238349915 + 0.0016307213764927449 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.978645384311676 + 0.0027064755458685794 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9905462265014648 + 0.001402578599690647 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9802553653717041 + 0.003173276637083633 * syst;
  }

  if (pt > 30 && pt <= 40) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.9861794114112854 + 0.0006187187412138267 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.9798933267593384 + 0.001057081371390319 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9923668503761292 + 0.0005653311393042486 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9785045385360718 + 0.0015542030446523895 * syst;
  }

  if (pt > 40 && pt <= 50) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.987443208694458 + 0.000494159746725046 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.980233907699585 + 0.000819615406448897 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9927627444267273 + 0.0004155573807947332 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9778544902801514 + 0.001456799997296391 * syst;
  }

  if (pt > 50 && pt <= 60) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.9834294319152832 + 0.0011818999573518245 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.9773300886154175 + 0.001955436343316424 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9886322021484375 + 0.0011254961157344963 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9654409885406494 + 0.003709169009223743 * syst;
  }

  if (pt > 60 && pt <= 120) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_id = 0.9863178730010986 + 0.002073330940717176 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_id = 0.9795225858688354 + 0.0035622593553725837 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_id = 0.9950451850891113 + 0.002673833447209764 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_id = 0.9689615368843079 + 0.011084748199568817 * syst;
  }

  if (abs(eta) > 2.4 || pt > 120) eff_id = 1;

  // additional 1% syst. uncertainty
  eff_id *= 1 + (0.01 * syst);


  // tight iso SF 
  if (pt > 20 && pt <= 25) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 1.0043761730194092 + 0.003959090391076143 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 1.004357933998108 + 0.006125539530136138 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 0.9970762133598328 + 0.003109125287470401 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 0.9957730770111084 + 0.006137193387970902 * syst;
  }

  if (pt > 25 && pt <= 30) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 0.9995378255844116 + 0.0022512071035640673 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 1.002331256866455 + 0.004003683572512011 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 1.0006532669067383 + 0.002067755362435184 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 0.9939026832580566 + 0.004261971076013437 * syst;
  }

  if (pt > 30 && pt <= 40) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 1.000901222229004 + 0.0007979481788689052 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 1.004658579826355 + 0.0014502638048416372 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 1.0023553371429443 + 0.0008445520691793605 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 0.997478187084198 + 0.001781225374381486 * syst;
  }

  if (pt > 40 && pt <= 50) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 0.9986253976821899 + 0.0004518361024064332 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 1.0013608932495117 + 0.0004888604573095644 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 0.999933660030365 + 0.0004309914887707696 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 1.002805233001709 + 0.001100242856214239 * syst;
  }

  if (pt > 50 && pt <= 60) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 1.0002487897872925 + 0.000772847340102783 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 0.9986217021942139 + 0.0012396364566794034 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 1.0002963542938232 + 0.0007614160360063238 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 1.0043764114379883 + 0.001806526581100641 * syst;
  }

  if (pt > 60 && pt <= 120) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_iso = 0.9986850023269653 + 0.0008907575174433545 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_iso = 1.0054655075073242 + 0.001589130019220112 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_iso = 1.0004935264587402 + 0.0009382223143922724 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_iso = 1.0010104179382324 + 0.0022795762936220253 * syst;
  }

  if (abs(eta) > 2.4 || pt > 120) eff_iso = 1;

  // additional 1% syst. uncertainty
  eff_iso *= 1 + (0.01 * syst);



  // IsoMu20 trigger SF

  /*
   This is not trivial as there are two sets of SF depending on the HLT menu.
   Values are the weighted average of both sets:

   total lumi:
   2197.95 pb-1

   HLT v4.2: run <= 257819
   393.47 pb-1
   0.179

   HLT v4.3: run > 257819
   1804.48 pb-1
   0.821
  */

  if (pt > 22 && pt <= 25) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.987956 + 0.0044549 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 1.0178482 + 0.0086129 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.9971603 + 0.0047654 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.0132546 + 0.0127635 * syst;
  }

  if (pt > 25 && pt <= 30) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.9953616 + 0.0023685 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 1.0096846 + 0.0048012 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.9969169 + 0.0028327 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.0136236 + 0.0073907 * syst;
  }

  if (pt > 30 && pt <= 40) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.9939314 + 0.0003506 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 1.0001498 + 0.0020148 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.98972 + 0.0013253 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.0178737 + 0.003679 * syst;
  }

  if (pt > 40 && pt <= 50) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.9952441 + 0.0033988 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 0.9939897 + 0.0015432 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.9892971 + 0.000579 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.0114995 + 0.0030685 * syst;
  }

  if (pt > 50 && pt <= 60) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.9939026 + 0.0014432 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 0.9977205 + 0.0243324 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.9889162 + 0.001979 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.019157 + 0.004705 * syst;
  }

  if (pt > 60 && pt <= 120) {
    if (abs(eta) > 0   && abs(eta) <= 0.9) eff_trigger = 0.9878582 + 0.0018148 * syst;
    if (abs(eta) > 0.9 && abs(eta) <= 1.2) eff_trigger = 0.9833346 + 0.0039938 * syst;
    if (abs(eta) > 1.2 && abs(eta) <= 2.1) eff_trigger = 0.9912019 + 0.0026506 * syst;
    if (abs(eta) > 2.1 && abs(eta) <= 2.4) eff_trigger = 1.0122248 + 0.0082802 * syst;
  }

  if (abs(eta) > 2.4 || pt > 120) eff_trigger = 1;

  // additional 0.5% syst. uncertainty
  eff_trigger *= 1 + (0.005 * syst);


  // total SF
  double eff_total = eff_id * eff_iso * eff_trigger;
  return eff_total;

}

void DMAnalysisTreeMaker::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup){

	iRun.getByToken(t_triggerNames_,triggerNames);
	iRun.getByToken(t_metNames_, metNames);
	
	if(triggerNames->size() > 0) edm::LogWarning("TriggerNames") << "List of triggers found";
	if(metNames->size() > 0) edm::LogWarning("METNames") << "List of met Filters found";
	for(unsigned int aa=0; aa< triggerNames->size(); aa++){
		std::cout<<triggerNames->at(aa)<<std::endl;
	}
}

void DMAnalysisTreeMaker::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup){

	if(triggerNames->size() > 0) triggerNames.clear();  
	if(metNames->size() > 0) metNames.clear();
}


//DMAnalysisTreeMaker::~DMAnalysisTreeMaker(const edm::ParameterSet& iConfig)
// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(DMAnalysisTreeMaker);
