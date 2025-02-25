/// ---------------------------------------------------------------------------
/*! \file   SLambdaJetHunter.h
 *  \author Derek Anderson
 *  \date   01.25.2024
 *
 *  A minimal analysis module to find lambda-tagged
 *  jets in pythia events.
 */
/// ---------------------------------------------------------------------------

#ifndef SLAMBDAJETHUNTER_H
#define SLAMBDAJETHUNTER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <map>
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
// root libraries
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
// hepmc libraries
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
// phool utilities
#include <phool/PHCompositeNode.h>
// f4a utilities
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
// PHG4 libraries
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4TruthInfoContainer.h>
// analysis utilities
#include <scorrelatorutilities/Tools.h>
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// analysis definitions
#include "SLambdaJetHunterConfig.h"

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Lambda-jet finder
  // --------------------------------------------------------------------------
  /*! A module to reconstruct jets at the generator level,
   *  and then tag those containing a strange lambda.
   */ 
  class SLambdaJetHunter : public SubsysReco {

    public:

      // options for how to associate jets & lambdas
      enum Associator { Barcode, Decay, Distance };

      // ctor/dtor
      SLambdaJetHunter(const string &name = "SLambdaJetHunter", const bool debug = false);
      SLambdaJetHunter(SLambdaJetHunterConfig& config);
      ~SLambdaJetHunter() override;

       // f4a methods
      int Init(PHCompositeNode *topNode)          override;
      int process_event(PHCompositeNode *topNode) override;
      int End(PHCompositeNode *topNode)           override;

      // setters
      void SetConfig(SLambdaJetHunterConfig& config) {m_config = config;}

      // getters
      SLambdaJetHunterConfig GetConfig() {return m_config;}

    private:

      // analysis methods (*.ana.h)
      void          GrabEventInfo(PHCompositeNode* topNode);
      void          FindLambdas(PHCompositeNode* topNode);
      void          MakeJets(PHCompositeNode* topNode);
      void          CollectJetOutput(PHCompositeNode* topNode);
      void          AssociateLambdasToJets(PHCompositeNode* topNode);
      void          FillOutputTree();
      bool          HasParentInfo(const int parent);
      bool          HasLambda(Types::JetInfo& jet);
      bool          IsGoodParticle(Types::ParInfo& particle);
      bool          IsGoodLambda(Types::ParInfo& lambda);
      bool          IsLambda(const int pid);
      bool          IsNewLambda(const int id);
      bool          IsInHepMCDecayChain(const int idToFind, HepMC::GenVertex* vtxToStart);
      bool          IsInPHG4DecayChain(const int idToFind, const int idLambda, PHCompositeNode* topNode);
      double        GetLambdaAssocZ(Types::ParInfo& lambda);
      double        GetLambdaAssocDr(Types::ParInfo& lambda);
      uint64_t      GetNTaggedJets();
      optional<int> HuntLambdasByBarcode(Types::ParInfo& lambda);
      optional<int> HuntLambdasByDecayChain(Types::ParInfo& lambda, PHCompositeNode* topNode);
      optional<int> HuntLambdasByDistance(Types::ParInfo& lambda);

      // system methods (*.sys.h)
      void InitTree();
      void InitOutput();
      void SaveAndCloseOutput();
      void ResetOutput();

      // i/o members
      TFile* m_outFile = NULL;
      TTree* m_outTree = NULL;

      // module configuration
      SLambdaJetHunterConfig m_config;

      // output variables
      Types::GEvtInfo                m_genEvtInfo;
      vector<Types::ParInfo>         m_lambdaInfo;
      vector<Types::JetInfo>         m_jetInfo;
      vector<vector<Types::CstInfo>> m_cstInfo;

      // vectors for internal calculations
      vector<int>               m_vecSubEvts;
      vector<int>               m_vecIDToCheck;
      vector<PseudoJet>         m_vecFastJets;
      vector<HepMC::GenVertex*> m_vecVtxToCheck;
      vector<HepMC::GenVertex*> m_vecVtxChecking;

      // jet-lambda associations
      map<int, int> m_mapLambdaJetAssoc;

      // output event variables
      //   - TODO move to dedicated class + interface
      uint64_t m_evtNJets;
      uint64_t m_evtNLambdas;
      uint64_t m_evtNTaggedJets;
      uint64_t m_evtNChrgPars;
      uint64_t m_evtNNeuPars;
      double   m_evtSumEPar;
      double   m_evtVtxX;
      double   m_evtVtxY;
      double   m_evtVtxZ;
      // output parton variables
      pair<int,   int>     m_evtPartID;
      pair<double, double> m_evtPartPx;
      pair<double, double> m_evtPartPy;
      pair<double, double> m_evtPartPz;
      pair<double, double> m_evtPartE;
      // output lambda variables
      vector<int>    m_lambdaID;
      vector<int>    m_lambdaPID;
      vector<int>    m_lambdaJetID;
      vector<int>    m_lambdaEmbedID;
      vector<double> m_lambdaZ;
      vector<double> m_lambdaDr;
      vector<double> m_lambdaE;
      vector<double> m_lambdaPt;
      vector<double> m_lambdaEta;
      vector<double> m_lambdaPhi;
      // output jet variables
      vector<bool>     m_jetHasLambda;
      vector<uint64_t> m_jetNCst;
      vector<uint64_t> m_jetID;
      vector<double>   m_jetE;
      vector<double>   m_jetPt;
      vector<double>   m_jetEta;
      vector<double>   m_jetPhi;
      // output constituent variables
      vector<vector<int>>    m_cstID;
      vector<vector<int>>    m_cstPID;
      vector<vector<int>>    m_cstJetID;
      vector<vector<int>>    m_cstEmbedID;
      vector<vector<double>> m_cstZ;
      vector<vector<double>> m_cstDr;
      vector<vector<double>> m_cstE;
      vector<vector<double>> m_cstPt;
      vector<vector<double>> m_cstEta;
      vector<vector<double>> m_cstPhi;

      // class-wide constants
      struct SLambdaJetHunterConsts {
        int pidLambda;
        int maxVtxToCheck;
      }  m_const = {3122, 500};

  };  // end SLambdaJetHunter

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
